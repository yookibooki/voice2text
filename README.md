# voice2text

Press `Alt+Space` to record, press again to paste the transcription where your cursor is.

One file, no daemon, works offline except for the transcription request to Groq.

---

## Get a key

Free at [console.groq.com/keys](https://console.groq.com/keys) → create `gsk_...` key.

```bash
echo 'export GROQ_API_KEY="gsk_..."' >> ~/.bashrc && source ~/.bashrc
```

---

## Linux

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

### 2. Link

```bash
ln -sf "$PWD/voice2text.sh" ~/.local/bin/voice2text
```

### 3. Hotkey

**i3 / Sway / Hyprland** — add to config:
```bash
bindsym Mod1+space exec --no-startup-id voice2text
```
Then `i3-msg reload` or `swaymsg reload`.

**GNOME** — Settings → Keyboard → Custom Shortcuts → `+` → Command `voice2text` → `Alt+Space`.

**KDE** — System Settings → Keyboard → Shortcuts → Add → Command `voice2text` → `Alt+Space`.

---

## macOS

No extra install — uses built-in `swift`, `curl`, `pbcopy`, `osascript`.

```bash
mkdir -p ~/bin
ln -sf "$PWD/voice2text-macos.sh" ~/bin/voice2text
echo 'export PATH="$HOME/bin:$PATH"' >> ~/.zshrc
echo 'export GROQ_API_KEY="gsk_..."' >> ~/.zshrc && source ~/.zshrc
```

Hotkey: Automator → Quick Action → Run Shell Script `~/bin/voice2text` → System Settings → Keyboard → Shortcuts → Services → assign `Alt+Space`.

---

## Windows

No extra install — uses built-in `curl.exe`, `winmm`, `Set-Clipboard`, `SendKeys`.

```powershell
mkdir $env:USERPROFILE\bin -Force
Copy-Item $PWD\voice2text.ps1 $env:USERPROFILE\bin\voice2text.ps1
setx PATH "$env:PATH;$env:USERPROFILE\bin"
setx GROQ_API_KEY "gsk_..."
```

Hotkey: Create shortcut to `powershell -File %USERPROFILE%\bin\voice2text.ps1` → Properties → Shortcut key `Alt+Space`.

---

## Use

1. Click a text field.
2. Press `Alt+Space` — speak.
3. Press `Alt+Space` again — text appears via `Ctrl+Shift+V`. It also stays in your clipboard.
> If a terminal is focused, text types at the prompt — focus your destination first.

---

## Uninstall

```bash
# Linux
rm ~/.local/bin/voice2text
# macOS
rm ~/bin/voice2text
```
```powershell
# Windows
Remove-Item $env:USERPROFILE\bin\voice2text.ps1
```

Then remove the `Alt+Space` shortcut from your window manager.
