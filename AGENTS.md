voice2text is a personal speech-to-text toggle (Alt+Space); first press records in background, second press transcribes via Groq Whisper and pastes.
Sole purpose:
1. Records audio via OS built-in (Linux ALSA, macOS AVFoundation, Windows winmm)
2. Sends to Groq's Whisper API (curl)
3. Gets transcription back
4. Pastes into focused window via OS clipboard (Linux xclip/wl-copy → xdotool/wtype, macOS pbcopy → osascript Cmd+V, Windows Set-Clipboard → SendKeys Ctrl+V)
Scripts (no build, no daemon, no comments):
- voice2text.sh (Linux, ~/.local/bin/voice2text): `arecord -q -t raw -f S16_LE -r 16000 -c 1 | opusenc --raw` → `DIR="${XDG_RUNTIME_DIR:-/tmp/v2t-$(id -u)}"; mkdir -p -m 700 "$DIR"; O="$DIR/v2t.ogg"` (0700 per-user tmpfs/RAM, no cp/symlink); `pkill -2 -f "arecord -q -t raw -f S16_LE -r 16000"` + `pgrep -f "opusenc.*$O"` wait; `curl -F file=@$O | jq`; `rm` after; `wtype -M ctrl -M shift -P v -p v -m shift -m ctrl` (precise, with fallback `wtype -M ctrl -P v -p v -m ctrl` / `xdotool key --clearmodifiers ctrl+shift+v || ctrl+v`)
- voice2text-macos.sh (macOS, ~/bin/voice2text): `swift AVAudioRecorder` (SIGINT → r.stop()) → `${TMPDIR:-/tmp}/v2t.wav` (per-user TMPDIR 0700, no cp/symlink); `pkill -2 -f "swift.*$O"`; `curl | python3 -c json`; `pbcopy` + `osascript`
- voice2text.ps1 (Windows, %USERPROFILE%\bin\voice2text.ps1): `winmm mciSendString waveaudio` → `$env:TEMP\v2t.wav` (per-user TEMP, flag file `$env:TEMP\v2t.rec` lock during curl, no cp/symlink); `curl.exe | ConvertFrom-Json`; `Set-Clipboard` + `SendKeys ^v`
Installers (built-in only, idempotent PATH, sanitized GROQ key, chmod 600 rc; only Windows creates hotkey):
- install.sh (Linux+macOS): `D=$(cd $(dirname $0) && pwd)` + `uname` branch, `mkdir -p`, `ln -sf $D/...`, `grep -q PATH || echo` to `~/.profile`+`~/.bashrc` (Linux) / `~/.zshrc` (macOS), `read -r K; Q=$(printf %s "$K" | sed "s/'/'\\\\''/g"); echo "export GROQ_API_KEY='$Q'"` + `chmod 600`; **does NOT create hotkey on Linux/macOS — manual bind required**
- install.ps1 (Windows): `Copy-Item` (no symlink), `[Environment]::GetEnvironmentVariable Path User` check + `setx` (needs new shell), `WScript.Shell` `Start Menu\voice2text.lnk` with `-WindowStyle Hidden` + `Hotkey Alt+Space`; note `ExecutionPolicy Bypass` may be needed
Deps (minimal, no ffmpeg/pulse/sox):
- Linux X11: alsa-utils, curl, jq, opus-tools, xdotool, xclip; Wayland: wtype, wl-clipboard
- macOS: swift (Xcode CLT via `xcode-select --install`), curl, python3, pbcopy, osascript (all built-in)
- Windows: curl.exe, winmm, Set-Clipboard, System.Windows.Forms (all built-in)
