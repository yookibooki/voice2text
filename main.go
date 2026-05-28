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

var version = "0.2.0"

const pidFile = "/tmp/voice2text.pid"
const wavFile = "/tmp/voice2text.wav"
const logFile = "/tmp/voice2text.log"

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

func log(msg string) {
	f, _ := os.OpenFile(logFile, os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0644)
	fmt.Fprintf(f, "%s %s\n", time.Now().Format("15:04:05"), msg)
	f.Close()
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
	log("start recording")
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
	return resp.Text, nil
}

func stopRecording(pid int) {
	syscall.Kill(-pid, syscall.SIGINT)
	if recordCmd != nil {
		recordCmd.Wait()
		recordCmd = nil
	}
	os.Remove(pidFile)
	log("stop recording, transcribing")

	apiKey := getAPIKey()
	if apiKey == "" {
		log("GROQ_API_KEY missing")
		return
	}

	client := openai.NewClient(
		option.WithAPIKey(apiKey),
		option.WithBaseURL("https://api.groq.com/openai/v1"),
	)

	models := []openai.AudioModel{"whisper-large-v3", "whisper-large-v3-turbo"}
	var text string
	for i, m := range models {
		t, err := transcribe(client, m)
		if err == nil {
			text = t
			break
		}
		log(fmt.Sprintf("model %s failed: %s", m, err.Error()))
		if strings.Contains(strings.ToLower(err.Error()), "rate limit") || strings.Contains(strings.ToLower(err.Error()), "429") {
			log(fmt.Sprintf("rate limited on %s, trying next model", m))
			continue
		}
		if i < len(models)-1 {
			log(fmt.Sprintf("non-rate-limit error on %s, trying next model", m))
			continue
		}
	}
	if text == "" {
		log("all models failed")
		return
	}

	log("result: " + text)
	typeText(text)
}

func typeText(text string) {
	if text == "" {
		return
	}
	ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
	defer cancel()
	if err := exec.CommandContext(ctx, "xdotool", "type", "--delay", "0", text).Run(); err != nil {
		log("type error: " + err.Error())
	}
}

func main() {
	if rec, pid := isRecording(); rec {
		stopRecording(pid)
	} else {
		startRecording()
	}
}
