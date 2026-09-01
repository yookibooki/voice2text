voice2text is a personal speech-to-text shell script (voice2text.sh, installed as ~/.local/bin/voice2text); triggered by Alt+Space (i3 bindsym); first press starts arecord in background, second press stops it and transcribes via Groq, then types the transcript via xdotool.
Sole purpose of this tool:
1. Records audio via ALSA
2. Sends it to Groq's Whisper API (via system curl)
3. Gets transcription back
4. Types transcript via xdotool type

- edit voice2text.sh directly, no build step