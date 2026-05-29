package main

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"sync"
	"syscall"
	"time"

	openai "github.com/openai/openai-go/v3"
	"github.com/openai/openai-go/v3/option"
)

const (
	pidFile = "/dev/shm/voice2text.pid" // RAM-backed, never touches disk
	wavFile = "/dev/shm/voice2text.wav" // RAM-backed
)

func getAPIKey() string {
	if k := os.Getenv("GROQ_API_KEY"); k != "" {
		return strings.TrimSpace(k)
	}
	path := filepath.Join(os.Getenv("HOME"), ".config", "voice2text", "groq.key")
	b, err := os.ReadFile(path)
	if err != nil {
		return ""
	}
	return strings.TrimSpace(string(b))
}

func isRecording() (bool, int) {
	data, err := os.ReadFile(pidFile)
	if err != nil {
		return false, 0
	}
	pid, _ := strconv.Atoi(strings.TrimSpace(string(data)))
	if pid <= 0 {
		return false, 0
	}
	err = syscall.Kill(pid, 0)
	return err == nil || err == syscall.EPERM, pid
}

func startRecording() {
	// Ensure we never accidentally reuse stale audio
	os.Remove(wavFile)

	cmd := exec.Command("arecord", "-f", "S16_LE", "-r", "16000", "-c", "1", wavFile)
	cmd.SysProcAttr = &syscall.SysProcAttr{Setpgid: true}
	if err := cmd.Start(); err != nil {
		fmt.Fprintln(os.Stderr, "arecord:", err)
		os.Exit(1)
	}
	os.WriteFile(pidFile, []byte(fmt.Sprintf("%d\n", cmd.Process.Pid)), 0644)
}

func transcribe(client openai.Client, model openai.AudioModel) (string, error) {
	f, err := os.Open(wavFile)
	if err != nil {
		return "", fmt.Errorf("open wav: %w", err)
	}
	defer f.Close()

	// Groq is fast; if it stalls, fail quickly and fall back
	ctx, cancel := context.WithTimeout(context.Background(), 8*time.Second)
	defer cancel()

	resp, err := client.Audio.Transcriptions.New(
		ctx,
		openai.AudioTranscriptionNewParams{
			Model: model,
			File:  f,
		},
	)
	if err != nil {
		return "", err
	}
	return strings.TrimSpace(resp.Text), nil
}

func typeText(text string) error {
	if text == "" {
		return nil
	}
	ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
	defer cancel()
	return exec.CommandContext(ctx, "xdotool", "type", "--delay", "0", text).Run()
}

func stopRecording(pid int) {
	// Build the API client concurrently while we stop arecord.
	// This removes client creation from the critical path.
	var client openai.Client
	var apiKey string
	var wg sync.WaitGroup
	wg.Add(1)
	go func() {
		defer wg.Done()
		apiKey = getAPIKey()
		if apiKey == "" {
			return
		}
		client = openai.NewClient(
			option.WithAPIKey(apiKey),
			option.WithBaseURL("https://api.groq.com/openai/v1"),
		)
	}()

	// Signal the whole process group so arecord finalizes the WAV header
	syscall.Kill(-pid, syscall.SIGINT)

	// arecord is not our child (we are a new process), so we can't syscall.Wait4.
	// Poll briefly — with /dev/shm this is enough.
	for i := 0; i < 200; i++ {
		if err := syscall.Kill(pid, 0); err != nil {
			break
		}
		time.Sleep(5 * time.Millisecond)
	}
	// If it survived SIGINT, kill it hard so we don't hang
	if syscall.Kill(pid, 0) == nil {
		syscall.Kill(-pid, syscall.SIGKILL)
		time.Sleep(10 * time.Millisecond)
	}

	os.Remove(pidFile)
	wg.Wait()
	if apiKey == "" {
		return
	}

	// Fastest model first. Only fall back on failure.
	models := []openai.AudioModel{"whisper-large-v3-turbo", "whisper-large-v3"}
	var text string
	for _, m := range models {
		t, err := transcribe(client, m)
		if err != nil {
			continue
		}
		text = t
		break
	}
	if text == "" {
		return
	}

	typeText(text)
}

func main() {
	if rec, pid := isRecording(); rec {
		stopRecording(pid)
	} else {
		startRecording()
	}
}
