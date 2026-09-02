# voice2text

Press `Alt+Space` to record, press again to paste the transcription where your cursor is.

One file, no daemon. Records locally, sends only the audio to Groq Whisper for transcription.

## Highlights

- Instant toggle — no waiting for app to open
- Cross-platform — Linux, macOS, Windows (one file per OS)
- Minimal — built-in tools only, no `ffmpeg`, `pulseaudio`, or daemon
- Private — audio deleted after upload (`rm`), key stays in your shell rc (`chmod 600`)

## Prerequisites

- Groq key: [console.groq.com/keys](https://console.groq.com/keys) → `gsk_...`
- Linux: `alsa-utils`, `opus-tools`, `curl`, `jq` + `xdotool`/`xclip` (X11) or `wtype`/`wl-clipboard` (Wayland)
- macOS: Xcode CLT for `swift` (`xcode-select --install`), plus `curl`, `python3` (both built-in)
- Windows: `curl.exe`, `winmm`, PowerShell (all built-in)

Check: `arecord -l` (Linux), `swift --version` (macOS), `curl.exe --version` (Windows)

## Installation

### Easy (recommended)

```bash
# Linux or macOS — one installer for both
./install.sh
source ~/.bashrc  # Linux, current shell only
source ~/.zshrc   # macOS, current shell only
```
```powershell
# Windows — right-click → Run with PowerShell
powershell -ExecutionPolicy Bypass -File .\install.ps1
```

What it does: `mkdir -p`, idempotent `PATH` append, prompts for `GROQ_API_KEY` (saved `chmod 600`), copies/symlinks the script. **Only Windows creates the `Alt+Space` shortcut** (`Start Menu\voice2text.lnk` with `-WindowStyle Hidden`). On Linux/macOS create the hotkey manually (below). Log out/in after install so hotkeys see the new `PATH`/`GROQ_API_KEY` (or use absolute path in shortcut).

### Manual — Linux

```bash
# X11
sudo apt install alsa-utils curl jq opus-tools xdotool xclip      # Debian/Ubuntu
sudo pacman -S alsa-utils curl jq opus-tools xdotool xclip        # Arch
# Wayland
sudo apt install alsa-utils curl jq opus-tools wtype wl-clipboard # Debian/Ubuntu
```
> `echo $XDG_SESSION_TYPE` → `x11` or `wayland`. GNOME/KDE Wayland need X11 session for paste.

```bash
mkdir -p ~/.local/bin
D="$(cd "$(dirname "$0")" && pwd)"; ln -sf "$D/voice2text.sh" ~/.local/bin/voice2text
grep -q '.local/bin' ~/.profile || echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.profile
grep -q '.local/bin' ~/.bashrc  || echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
grep -q GROQ_API_KEY ~/.bashrc || echo 'export GROQ_API_KEY="gsk_..."' >> ~/.bashrc; chmod 600 ~/.bashrc ~/.profile; source ~/.bashrc
```

Hotkey: `bindsym Mod1+space exec --no-startup-id voice2text` → `i3-msg reload` (or GNOME Settings → Keyboard → Custom → `Alt+Space`).

### Manual — macOS

```bash
mkdir -p ~/bin
D="$(cd "$(dirname "$0")" && pwd)"; ln -sf "$D/voice2text-macos.sh" ~/bin/voice2text
grep -q 'HOME/bin' ~/.zshrc || echo 'export PATH="$HOME/bin:$PATH"' >> ~/.zshrc
echo 'export GROQ_API_KEY="gsk_..."' >> ~/.zshrc; chmod 600 ~/.zshrc; source ~/.zshrc
```
> If `swift: command not found` → `xcode-select --install`. Use `$HOME/bin/voice2text` in Automator ( `~` doesn't expand).

Hotkey: Automator → Quick Action → Run Shell Script `$HOME/bin/voice2text` → System Settings → Keyboard → Shortcuts → Services → `Alt+Space`. Grant Microphone + Accessibility when prompted.

### Manual — Windows

```powershell
mkdir $env:USERPROFILE\bin -Force
Copy-Item $PSScriptRoot\voice2text.ps1 $env:USERPROFILE\bin\voice2text.ps1 -Force
$P=[Environment]::GetEnvironmentVariable("Path","User"); if ($P -notlike "*$env:USERPROFILE\bin*") {setx PATH "$P;$env:USERPROFILE\bin"}
setx GROQ_API_KEY "gsk_..."
```
> Close and reopen PowerShell after `setx`. If `Running scripts is disabled` → `Set-ExecutionPolicy -Scope CurrentUser RemoteSigned`.

Hotkey: `install.ps1` creates `%APPDATA%\Microsoft\Windows\Start Menu\Programs\voice2text.lnk` → `powershell.exe -WindowStyle Hidden -File %USERPROFILE%\bin\voice2text.ps1` with `Alt+Space`. For manual, create shortcut there with same Target and `-WindowStyle Hidden`.

### Verify

```bash
which voice2text          # Linux/macOS
echo $GROQ_API_KEY | cut -c1-8  # should show gsk_...
# Windows
Get-Command voice2text.ps1; echo $env:GROQ_API_KEY
```

## Usage

1. Click a text field.
2. Press `Alt+Space` — speak.
3. Press `Alt+Space` again — text appears (Linux `Ctrl+Shift+V`, macOS `Cmd+V`, Windows `Ctrl+V`) and stays in clipboard.

> If a terminal is focused, text types at the prompt — focus your destination first.

## Configuration

- Change Groq model: edit `model=whisper-large-v3-turbo` in `voice2text.sh:14`
- Re-record cancels via second press during `arecord`; early `GROQ_API_KEY` missing exits silently — set key first.

## Docs

- Agent guide: `AGENTS.md`
- Scripts: `voice2text.sh:raw`, `voice2text-macos.sh:raw`, `voice2text.ps1:raw`
- Installers: `install.sh:raw`, `install.ps1:raw`

## License

MIT — see `LICENSE` (or treat as personal script, no warranty).

## Acknowledgements

Built on `arecord`/`opusenc`, Groq Whisper, `xclip`/`xdotool`/`wtype`, `swift`/`AVFoundation`, `winmm`. Thanks to testers on i3, Sway, and Hyprland.
