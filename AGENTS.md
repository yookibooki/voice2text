voice2text is a personal speech-to-text toggle (Alt+Space); first press records in background, second press transcribes via Groq Whisper and pastes.
Sole purpose:
1. Records audio via OS built-in (Linux ALSA, macOS AVFoundation, Windows winmm)
2. Sends to Groq's Whisper API (curl)
3. Gets transcription back
4. Pastes into focused window via OS clipboard (Linux xclip/wl-copy → xdotool/wtype, macOS pbcopy → osascript Cmd+V, Windows Set-Clipboard → SendKeys Ctrl+V)
Scripts (no build, no daemon, no comments):
- voice2text.sh (Linux, ~/.local/bin/voice2text): `arecord -q -t raw -f S16_LE -r 16000 -c 1 | opusenc --raw` → `DIR="${XDG_RUNTIME_DIR:-/tmp/v2t-$(id -u)}"; mkdir -p -m 700 "$DIR"; O="$DIR/v2t.ogg"` (0700 per-user tmpfs/RAM, no cp/symlink); `pkill -2 arecord -q -t raw -f S16_LE -r 16000` + `pgrep opusenc` wait; `curl -F file=@$O | jq`; `rm` after; `wtype -M ctrl -M shift -P v -p v` (precise)
- voice2text-macos.sh (macOS, ~/bin/voice2text): `swift AVAudioRecorder` (SIGINT → r.stop()) → `${TMPDIR:-/tmp}/v2t.wav` (per-user TMPDIR 0700, no cp); `pkill -2 swift`; `curl | python3 -c json`; `pbcopy` + `osascript`
- voice2text.ps1 (Windows, %USERPROFILE%\bin\voice2text.ps1): `winmm mciSendString waveaudio` → `$env:TEMP\v2t.wav` (per-user TEMP, flag file lock during curl, no cp); `curl.exe | ConvertFrom-Json`; `Set-Clipboard` + `SendKeys ^v`
Installers (built-in only, idempotent PATH, sanitized GROQ key, chmod 600 rc):
- install.sh (Linux+macOS): `D=$(cd $(dirname $0) && pwd)` + `uname` branch, `mkdir -p`, `ln -sf $D/...`, `grep -q PATH || echo`, `read -r K; Q=$(sed s/'/'\\\\''/g); echo 'export GROQ_API_KEY='\''$Q'\'''`
- install.ps1 (Windows): `Copy-Item`, `[Environment]::GetEnvironmentVariable Path User` check + `setx`, `WScript.Shell` `Start Menu\voice2text.lnk` with `-WindowStyle Hidden` + `Hotkey Alt+Space`
Deps (minimal, no ffmpeg/pulse/sox):
- Linux X11: alsa-utils, curl, jq, opus-tools, xdotool, xclip; Wayland: wtype, wl-clipboard
- macOS: swift (CLT), curl, python3, pbcopy, osascript (all built-in)
- Windows: curl.exe, winmm, Set-Clipboard, System.Windows.Forms (all built-in)
