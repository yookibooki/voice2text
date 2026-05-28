# voice2text

ALT+SPACE to dictate on Linux. Press once to record, press again to transcribe with Groq Whisper.

## Install

```bash
# Build
go build -o ~/.local/bin/voice2text

# API key
mkdir -p ~/.config/voice2text
echo "gsk_your_key_here" > ~/.config/voice2text/groq.key
chmod 600 ~/.config/voice2text/groq.key
```

## Usage

```bash
voice2text   # toggle recording on/off → transcribes to cursor via xdotool
```

## Setup

See [SETUP.md](SETUP.md) — it covers other desktop environments and wms.

## Requirements

- `arecord` (alsa-utils) — recording
- `xdotool` — typing transcribed text
- X11 (for xdotool typing)
- Groq API key with whisper-large-v3 access
