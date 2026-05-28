voice2text is a personal speech-to-text tool; triggered by ALT+SPACE (i3); it toggles voice recording, sends audio to Groq's Whisper API.

- run after changes: `goimports -w . && go mod tidy && go build -o ~/.local/bin/voice2text`
- API key stored at `~/.config/voice2text/groq.key`
