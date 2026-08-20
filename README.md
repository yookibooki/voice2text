# voice2text

Voice-to-text for Linux via Groq's Whisper API. Records audio on demand, transcribes it, and pastes the result into the active window.

## Usage

Press `ALT+SPACE` (bind this in your window manager) to toggle recording. Transcribed text is pasted via `xdotool` (`Ctrl+Shift+V`).

```bash
# build
gcc -O3 voice2text.c -o ~/.local/bin/voice2text -lasound -lcurl

# set API key
echo "gsk_..." > ~/.config/voice2text/groq.key
```

## Requirements

- Linux with ALSA (`libasound-dev`)
- `libcurl` (`libcurl4-openssl-dev`)
- `xdotool`, `xclip`
- A [Groq API key](https://console.groq.com/keys) (free tier available)

Bind `ALT+SPACE` → `voice2text` in i3/sway/whatever.
