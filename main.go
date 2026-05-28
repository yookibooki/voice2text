package main

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"syscall"
	"time"

	openai "github.com/openai/openai-go/v3"
	"github.com/openai/openai-go/v3/option"
)

const pidFile = "/tmp/voice2text.pid"
const wavFile = "/tmp/voice2text.wav"

var recordCmd *exec.Cmd

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
	pid, _ := strconv.Atoi(string(data))
	return syscall.Kill(pid, 0) == nil, pid
}

func startRecording() {
	cmd := exec.Command("arecord", "-f", "S16_LE", "-r", "16000", "-c", "1", wavFile)
	cmd.SysProcAttr = &syscall.SysProcAttr{Setpgid: true}
	cmd.Start()
	recordCmd = cmd
	os.WriteFile(pidFile, []byte(fmt.Sprintf("%d", cmd.Process.Pid)), 0644)
}

func transcribe(client openai.Client, model openai.AudioModel) (string, error) {
	f, err := os.Open(wavFile)
	if err != nil {
		return "", fmt.Errorf("open wav: %w", err)
	}
	defer f.Close()

	resp, err := client.Audio.Transcriptions.New(
		context.Background(),
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

func stopRecording(pid int) {
	syscall.Kill(-pid, syscall.SIGINT)
	if recordCmd != nil {
		recordCmd.Wait()
		recordCmd = nil
	}
	os.Remove(pidFile)
	apiKey := getAPIKey()
	if apiKey == "" {
		return
	}

	client := openai.NewClient(
		option.WithAPIKey(apiKey),
		option.WithBaseURL("https://api.groq.com/openai/v1"),
	)

	models := []openai.AudioModel{"whisper-large-v3", "whisper-large-v3-turbo"}
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

	if err := typeText(text); err != nil {
		fmt.Fprintf(os.Stderr, "Failed to type text: %v\n", err)
	}
}

func typeText(text string) error {
	if text == "" {
		return nil
	}
	ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
	defer cancel()
	return exec.CommandContext(ctx, "xdotool", "type", "--delay", "0", text).Run()
}

func main() {
	if rec, pid := isRecording(); rec {
		stopRecording(pid)
	} else {
		startRecording()
	}
}
