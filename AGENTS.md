voice2text is a personal speech-to-text shell script (voice2text.sh, installed as ~/.local/bin/voice2text); triggered by the i3 binding `bindsym Mod1+space exec --no-startup-id voice2text` (Alt+Space); first press starts `arecord -q -t raw | opusenc` in background, second press stops it (`pkill -2 arecord`, wait `opusenc`) and transcribes via Groq, then pastes the transcript via `xclip → xdotool ctrl+shift+v` (X11) / `wl-copy → wtype ctrl+shift+v` (Wayland).
Sole purpose of this tool:
1. Records audio via ALSA
2. Sends it to Groq's Whisper API (via system curl)
3. Gets transcription back
4. Pastes transcript into the focused window (xclip → xdotool ctrl+shift+v on X11; wl-copy → wtype ctrl+shift+v on Wayland)

- edit voice2text.sh directly, no build step
- Records to /tmp/v2t.ogg as Opus-in-OGG (chmod 600 via umask 077, deleted after curl). Via `arecord -q -t raw -f S16_LE -r 16000 -c 1 | opusenc --quiet --raw --raw-bits 16 --raw-rate 16000 --raw-chan 1`. Fixes the arecord-SIGTERM-corrupts-WAV-header bug.
- X11 deps: alsa-utils, curl, jq, opus-tools, xdotool, xclip
- Wayland deps: alsa-utils, curl, jq, opus-tools, wtype, wl-clipboard
