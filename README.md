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
source ~/.bashrc  # Linux (current shell only)
source ~/.zshrc   # macOS (current shell only)
```
```powershell
# Windows — right-click Run with PowerShell, or:
powershell -ExecutionPolicy Bypass -File .\install.ps1
```

What the installers do: `mkdir -p`, idempotent `PATH` append, `GROQ_API_KEY` prompt (saved with `chmod 600` on Unix), and symlink/copy of the script. **Only Windows creates the `Alt+Space` shortcut** (`%APPDATA%\Microsoft\Windows\Start Menu\Programs\voice2text.lnk` with `-WindowStyle Hidden` and `Hotkey Alt+Space`). **On Linux and macOS you still need to create the hotkey manually** — see sections below.

> `source ~/.bashrc` / `source ~/.zshrc` only affects the current terminal. Window managers, desktop hotkeys, and Automator services read `~/.profile` / `~/.zshrc` at login, so **log out and back in** (or reload your WM: `i3-msg reload` / `swaymsg reload`) for the new `PATH`/`GROQ_API_KEY` to be visible to the hotkey. Alternatively set the hotkey command to the absolute path (`$HOME/.local/bin/voice2text` or `$HOME/bin/voice2text`).

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

### 2. Link and PATH

Run from the cloned repo directory (don't copy the `$PWD` line blindly — `$PWD` is fragile if you move the repo; `install.sh` uses `D="$(cd "$(dirname "$0")" && pwd)"` for a repo-relative absolute path):

```bash
mkdir -p ~/.local/bin
D="$(cd "$(dirname "$0")" && pwd)"
ln -sf "$D/voice2text.sh" ~/.local/bin/voice2text
# idempotent PATH — add to BOTH files: .profile is read at login (hotkeys), .bashrc at interactive shells
grep -q '.local/bin' ~/.profile 2>/dev/null || echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.profile
grep -q '.local/bin' ~/.bashrc 2>/dev/null || echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
```

### 3. API key

```bash
# add to both so hotkeys (login shell) and terminals see it; keep rc private
echo 'export GROQ_API_KEY="gsk_..."' >> ~/.bashrc; chmod 600 ~/.bashrc
grep -q GROQ_API_KEY ~/.profile 2>/dev/null || echo 'export GROQ_API_KEY="gsk_..."' >> ~/.profile; chmod 600 ~/.profile
# current shell only — hotkey needs logout/login or WM reload
source ~/.bashrc
```

### 4. Hotkey

```bash
bindsym Mod1+space exec --no-startup-id voice2text
```
Then `i3-msg reload` or `swaymsg reload`.

**GNOME** — Settings → Keyboard → Custom Shortcuts → `+` → Command `voice2text` (or `$HOME/.local/bin/voice2text` if PATH not yet reloaded) → `Alt+Space`.

**KDE** — System Settings → Keyboard → Shortcuts → Add → Command `voice2text` → `Alt+Space`.

> If the hotkey says "command not found", log out/in so `~/.profile` PATH is picked up, or use the absolute path in the shortcut.

---

## macOS — manual

No extra install — uses built-in `curl`, `python3`, `pbcopy`, `osascript`, and `swift` (requires **Xcode Command Line Tools**; if `swift` is missing run `xcode-select --install` and follow the prompt).

```bash
mkdir -p ~/bin
D="$(cd "$(dirname "$0")" && pwd)"
ln -sf "$D/voice2text-macos.sh" ~/bin/voice2text
grep -q 'HOME/bin' ~/.zshrc 2>/dev/null || echo 'export PATH="$HOME/bin:$PATH"' >> ~/.zshrc
echo 'export GROQ_API_KEY="gsk_..."' >> ~/.zshrc; chmod 600 ~/.zshrc; source ~/.zshrc
# source only affects current shell — log out/in for Automator/Services to see new env
```

Hotkey: Automator → Quick Action → Run Shell Script → enter `$HOME/bin/voice2text` (don't use `~/bin/voice2text` — `~` does **not** expand in Automator's shell) → System Settings → Keyboard → Shortcuts → Services → assign `Alt+Space`.

**Permissions** (required on first run): System Settings → Privacy & Security → **Microphone** and **Accessibility** → allow `Terminal` / `Automator` / the app running the script. Without these, recording is silent and paste is blocked.

---

## Windows — manual

No extra install — uses built-in `curl.exe`, `winmm`, `Set-Clipboard`, `SendKeys`.

```powershell
mkdir $env:USERPROFILE\bin -Force
Copy-Item $PSScriptRoot\voice2text.ps1 $env:USERPROFILE\bin\voice2text.ps1 -Force
# or if running manually from repo: Copy-Item $PWD\voice2text.ps1 $env:USERPROFILE\bin\voice2text.ps1 -Force
$P=[Environment]::GetEnvironmentVariable("Path","User"); if ($P -notlike "*$env:USERPROFILE\bin*") {setx PATH "$P;$env:USERPROFILE\bin" | Out-Null}
setx GROQ_API_KEY "gsk_..." | Out-Null
```

Hotkey: `install.ps1` creates `%APPDATA%\Microsoft\Windows\Start Menu\Programs\voice2text.lnk` → `powershell.exe -WindowStyle Hidden -File %USERPROFILE%\bin\voice2text.ps1` with `Alt+Space`. For manual: create shortcut there, set Target as above, add `-WindowStyle Hidden` to avoid console flash, then assign `Alt+Space`.

> **ExecutionPolicy**: if scripts are blocked, run `Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy Bypass` or launch with `powershell -ExecutionPolicy Bypass -File .\install.ps1` / `powershell -ExecutionPolicy Bypass -File $env:USERPROFILE\bin\voice2text.ps1`.
> **`setx` needs a new shell**: it writes to the registry and does **not** affect the current terminal — close and reopen PowerShell (or log out/in) for new `PATH`/`GROQ_API_KEY` to be visible. `install.ps1` also updates `$env:PATH` for the current session only.

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
# then edit ~/.bashrc, ~/.profile (Linux) / ~/.zshrc (macOS) to remove PATH and GROQ_API_KEY lines, chmod 600 if needed
```
```powershell
# Windows
Remove-Item $env:USERPROFILE\bin\voice2text.ps1
Remove-Item "$env:APPDATA\Microsoft\Windows\Start Menu\Programs\voice2text.lnk"
# then remove PATH entry and GROQ_API_KEY via System → Environment Variables (or setx), and restart shell
```

Then remove the `Alt+Space` shortcut (Linux window manager, macOS Automator service, Windows `.lnk`).
