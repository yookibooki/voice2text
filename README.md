# voice2text

`Alt+Space` to record → `Alt+Space` to paste transcription where your cursor is.

## Install

Get a free key: [console.groq.com/keys](https://console.groq.com/keys) → `gsk_...`

```bash
# Linux or macOS
./install.sh          # asks for GROQ_API_KEY, adds to shell rc
source ~/.bashrc      # or ~/.zshrc on macOS, then log out/in for hotkey
```
```powershell
# Windows — right-click → Run with PowerShell
powershell -ExecutionPolicy Bypass -File .\install.ps1
```

Requires: Linux `alsa-utils opus-tools curl jq xdotool/xclip` (or `wtype/wl-clipboard` on Wayland), macOS `swift` (`xcode-select --install` if missing) + `curl`/`python3`, Windows built-in.

## Use

1. Click a text field
2. Press `Alt+Space`, speak
3. Press `Alt+Space` again — text appears (`Ctrl+Shift+V` / `Cmd+V` / `Ctrl+V`)

Manual hotkey if Easy Install didn't create it: `bindsym Mod1+space exec voice2text` (i3) or Automator Quick Action `$HOME/bin/voice2text` (macOS) or `Start Menu\voice2text.lnk` → `Alt+Space` (Windows).

## Uninstall

```bash
rm ~/.local/bin/voice2text  # Linux
rm ~/bin/voice2text         # macOS
```
```powershell
Remove-Item $env:USERPROFILE\bin\voice2text.ps1  # Windows
```
