# voice2text

Press `Alt+Space` to record, press again to paste the transcription where your cursor is.

One file, no daemon, works offline except for the transcription request to Groq.

---

## Get a key

Free at [console.groq.com/keys](https://console.groq.com/keys) → create `gsk_...` key. Or let the installer prompt you.

---

## Easy install

```bash
# Linux or macOS — one file for both
./install.sh
source ~/.bashrc  # Linux
source ~/.zshrc   # macOS
```
```powershell
# Windows — right-click Run with PowerShell
.\install.ps1
```
Installers do `mkdir -p`, idempotent `PATH` append, `GROQ_API_KEY` prompt, and create the `Alt+Space` shortcut (Windows `Start Menu\voice2text.lnk` with `-WindowStyle Hidden`).

---

## Linux — manual

### 1. Install

**X11** (i3, XFCE, Cinnamon, LXQt, GNOME/KDE on X11):
```bash
# Debian/Ubuntu
sudo apt install alsa-utils curl jq opus-tools xdotool xclip
# Arch
sudo pacman -S alsa-utils curl jq opus-tools xdotool xclip
# Fedora
sudo dnf install alsa-utils curl jq opus-tools xdotool xclip
```

**Wayland** (Sway, Hyprland, Wayfire, River):
```bash
# Debian/Ubuntu
sudo apt install alsa-utils curl jq opus-tools wtype wl-clipboard
# Arch
sudo pacman -S alsa-utils curl jq opus-tools wtype wl-clipboard
# Fedora
sudo dnf install alsa-utils curl jq opus-tools wtype wl-clipboard
```

> Not sure? Run `echo $XDG_SESSION_TYPE` — it prints `x11` or `wayland`.
> GNOME Wayland and KDE Wayland need an X11 session for paste to work.

### 2. Link (creates dir if needed)

 ```bash
mkdir -p ~/.local/bin
 ln -sf "$PWD/voice2text.sh" ~/.local/bin/voice2text
grep -q '.local/bin' ~/.bashrc || echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
 ```

 ### 3. Hotkey
```bash
bindsym Mod1+space exec --no-startup-id voice2text
```
Then `i3-msg reload` or `swaymsg reload`.

**GNOME** — Settings → Keyboard → Custom Shortcuts → `+` → Command `voice2text` → `Alt+Space`.

**KDE** — System Settings → Keyboard → Shortcuts → Add → Command `voice2text` → `Alt+Space`.

---

## macOS — manual

No extra install — uses built-in `swift`, `curl`, `python3`, `pbcopy`, `osascript`.

```bash
mkdir -p ~/bin
ln -sf "$PWD/voice2text-macos.sh" ~/bin/voice2text
echo 'export PATH="$HOME/bin:$PATH"' >> ~/.zshrc
echo 'export GROQ_API_KEY="gsk_..."' >> ~/.zshrc; chmod 600 ~/.zshrc; source ~/.zshrc
```

Hotkey: Automator → Quick Action → Run Shell Script `~/bin/voice2text` → System Settings → Keyboard → Shortcuts → Services → assign `Alt+Space`.

---
## Windows — manual

No extra install — uses built-in `curl.exe`, `winmm`, `Set-Clipboard`, `SendKeys`.

```powershell
mkdir $env:USERPROFILE\bin -Force
Copy-Item $PWD\voice2text.ps1 $env:USERPROFILE\bin\voice2text.ps1
$P=[Environment]::GetEnvironmentVariable("Path","User"); if ($P -notlike "*$env:USERPROFILE\bin*") {setx PATH "$P;$env:USERPROFILE\bin"}
setx GROQ_API_KEY "gsk_..."
```

Hotkey: `install.ps1` creates `%APPDATA%\Microsoft\Windows\Start Menu\Programs\voice2text.lnk` → `powershell.exe -WindowStyle Hidden -File %USERPROFILE%\bin\voice2text.ps1` with `Alt+Space`. For manual: create shortcut there, set Target as above, add `-WindowStyle Hidden` to avoid console flash, then assign `Alt+Space`.

---
## Use

1. Click a text field.
2. Press `Alt+Space` — speak.
3. Press `Alt+Space` again — text appears and stays in clipboard:
   - Linux: `Ctrl+Shift+V` (fallback `Ctrl+V` via `xdotool`/`wtype`)
   - macOS: `Cmd+V` via `pbcopy` + `osascript`
   - Windows: `Ctrl+V` via `Set-Clipboard` + `SendKeys`
> If a terminal is focused, text types at the prompt — focus your destination first.

---

## Uninstall

```bash
# Linux
rm ~/.local/bin/voice2text
# macOS
rm ~/bin/voice2text
# then edit ~/.bashrc / ~/.zshrc / ~/.profile to remove PATH and GROQ_API_KEY lines, chmod 600 if needed
```
```powershell
# Windows
Remove-Item $env:USERPROFILE\bin\voice2text.ps1
Remove-Item "$env:APPDATA\Microsoft\Windows\Start Menu\Programs\voice2text.lnk"
# then remove PATH entry and GROQ_API_KEY via System → Environment Variables
```

Then remove the `Alt+Space` shortcut (Linux window manager, macOS Automator service, Windows `.lnk`).
