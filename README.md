# voice2text

`Alt+Space` to record → `Alt+Space` to paste transcription where your cursor is.

## Install

Get a free key: [console.groq.com/keys](https://console.groq.com/keys) → `gsk_...`

```bash
curl -fsSL https://raw.githubusercontent.com/yookibooki/voice2text/main/install.sh | sh
```
```powershell
irm https://raw.githubusercontent.com/yookibooki/voice2text/main/install.ps1 | iex
```

Requires: Linux `alsa-utils opus-tools curl jq xdotool/xclip` (or `wtype/wl-clipboard`), macOS `swift` (`xcode-select --install` if missing), Windows built-in.


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
