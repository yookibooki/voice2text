voice2text is a personal speech-to-text tool; triggered by ALT+SPACE (i3); it toggles voice recording, sends audio to Groq's Whisper API.
Sole purpose of this tool:
1. Records audio via ALSA
2. Sends it to Groq's Whisper API (via system curl)
3. Gets transcription back
4. Pastes via xdotool (Ctrl+V simulation) + xclip (clipboard)

- run after changes: `gcc -O3 voice2text.c -o ~/.local/bin/voice2text -lasound -lcurl`
- API key stored at `~/.config/voice2text/groq.key`
