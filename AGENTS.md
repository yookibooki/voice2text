# AGENTS — voice2text

One-file speech-to-text toggle: `Alt+Space` records, second press transcribes via Groq Whisper (`whisper-large-v3-turbo`) and pastes.

## WHAT

- `voice2text.sh` (Linux, `~/.local/bin/voice2text`): `arecord -q -t raw | opusenc` → `DIR="${XDG_RUNTIME_DIR:-/tmp/v2t-$(id -u)}"/v2t.ogg` (0700, tmpfs)
- `voice2text-macos.sh` (macOS, `~/bin/voice2text`): `swift AVAudioRecorder` → `${TMPDIR:-/tmp}/v2t.wav` (per-user TMPDIR)
- `voice2text.ps1` (Windows, `%USERPROFILE%\bin`): `winmm mciSendString` → `$env:TEMP\v2t.wav`
- `install.sh` (Linux+macOS) + `install.ps1` (Windows): idempotent `PATH`, sanitized `GROQ_API_KEY`, `chmod 600` (Unix), `WScript.Shell .lnk` (Win, only hotkey creator)
- Stack: `sh`/`powershell`, `curl`/`curl.exe`, `jq`/`python3`/`ConvertFrom-Json`, `xclip`/`wl-copy`+`xdotool`/`wtype`, `pbcopy`+`osascript`, `Set-Clipboard`+`SendKeys`

## WHY

Personal, minimal, built-in only — no daemon, no `ffmpeg`/`pulse`/`sox`. See `voice2text.sh:raw`, `voice2text-macos.sh:raw`, `voice2text.ps1:raw`.

## HOW

No build. Edit scripts directly.

Verify:
```sh
bash -x ./voice2text.sh & sleep 0.5; pgrep -a arecord; pkill -2 -f "arecord -q -t raw -f S16_LE -r 16000"; sleep 0.5; ls -l ${XDG_RUNTIME_DIR:-/tmp}/v2t-*/v2t.ogg; curl -sS $API -H "Bearer $GROQ_API_KEY" -F file=@... | jq
```
Test paste: `printf test | xclip -selection clipboard; xclip -o`

## Style

- No comments in code (shebang only), minimal, `sh`/`ps1` built-ins only
- `umask 077`, `DIR` 0700, per-user tmp, `pkill -2` + `pgrep` wait, `rm` after `curl`
- Paste: `xclip→xdotool ctrl+shift+v || ctrl+v` / `wl-copy→wtype -M ctrl -M shift -P v -p v` / `pbcopy→osascript Cmd+V` / `Set-Clipboard→SendKeys ^v`

## Boundaries

- Never commit `GROQ_API_KEY` or `gsk_...`; never add `ffmpeg`/`pulse`/`sox`/daemon/comments
- Never touch `~/.local/bin` on macOS/Windows (use `~/bin` / `%USERPROFILE%\bin`)
- Keep installers idempotent: `grep -q PATH || echo`, `D=$(cd $(dirname $0) && pwd)`, `read -r` + `sed "s/'/'\\\\''/g"` + `chmod 600`

## Git
- Commits: `vX.Y.Z: short` + bullets
- Docs: `README.md` for end users (copy-paste install), `AGENTS.md` for agents (this file) — single source, no duplication
