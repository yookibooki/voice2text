# voice2text

Dictation toggle for Linux. Press the hotkey to record, press again to transcribe and paste.

Fork-per-press shell script. No daemon, no global key grabs, no lock files. The window manager owns the hotkeys.

## Usage

```bash
cp voice2text.sh ~/.local/bin/voice2text && chmod +x ~/.local/bin/voice2text
export GROQ_API_KEY="gsk_..."
```

```bash
bindsym Mod1+Space exec --no-startup-id voice2text
```

Press `Alt+Space` → recording starts. Press again → the audio goes to Groq and the transcript is typed into the focused window.

## How it works

1. First press: `arecord` (16 kHz mono S16LE) captures to `/tmp/voice2text.wav`, its pid goes to `/tmp/voice2text.pid`
2. Second press: the pidfile pid is signalled; on next press a new capture overwrites the file
3. `curl` POSTs the WAV to Groq's transcription API; `jq` extracts the text
4. `xdotool type` synthesises the keystrokes into the focused window

Requirements: `alsa-utils`, `curl`, `jq`, `xdotool`. API key via `$GROQ_API_KEY`. Transcription language: English.