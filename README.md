# voice2text

Press `Alt+Space` to record. Press again to transcribe and paste where your cursor is.

One shell script. No daemon. No background service.

`arecord -q -t raw | opusenc` → Groq Whisper (`whisper-large-v3-turbo`) → `xclip`/`wl-copy` → `xdotool`/`wtype` `Ctrl+Shift+V` — records to `/tmp/v2t.ogg` (Opus-in-OGG, `umask 077`, deleted after upload)

---

## Requirements — only these (`procps` for `pkill`/`pgrep` is standard)

- Base (both): `alsa-utils` (`arecord`), `opus-tools` (`opusenc`), `curl`, `jq`
- **X11** (i3, XFCE, Cinnamon, LXQt, GNOME/KDE on X11): `xdotool` `xclip`
- **Wayland** (Sway, Hyprland, Wayfire, River, COSMIC): `wtype` `wl-clipboard`

No `ffmpeg`, `pulseaudio`/`pipewire`, `sox`, or daemon — `arecord -q -t raw | opusenc --raw` writes `/tmp/v2t.ogg` directly.
> GNOME Wayland (Mutter) and KDE Plasma Wayland (KWin) don't expose the virtual-keyboard protocol `wtype` needs. Use an X11 session on those.

Get a free API key at [console.groq.com/keys](https://console.groq.com/keys).

---

## Install

**1. Dependencies — pick your distro**

Debian / Ubuntu

```bash
sudo apt install alsa-utils curl jq opus-tools xdotool xclip      # X11
# or
sudo apt install alsa-utils curl jq opus-tools wtype wl-clipboard # Wayland
```

Arch Linux

```bash
sudo pacman -S alsa-utils curl jq opus-tools xdotool xclip      # X11
# or
sudo pacman -S alsa-utils curl jq opus-tools wtype wl-clipboard # Wayland
```

Fedora

```bash
sudo dnf install alsa-utils curl jq opus-tools xdotool xclip      # X11
# or
sudo dnf install alsa-utils curl jq opus-tools wtype wl-clipboard # Wayland
```

**2. Link the script**

```bash
ln -sf "$PWD/voice2text.sh" ~/.local/bin/voice2text
```

**3. Add your key**

```bash
echo 'export GROQ_API_KEY="gsk_..."' >> ~/.bashrc && source ~/.bashrc
```

Add the hotkey:

**i3 / Sway / Hyprland**

```bash
# ~/.config/i3/config or ~/.config/sway/config
bindsym Mod1+space exec --no-startup-id voice2text
# then: i3-msg reload  /  swaymsg reload
```

**GNOME** — Settings → Keyboard → View and Customize Shortcuts → Custom Shortcuts → `+` → Command: `voice2text` → Shortcut: `Alt+Space`

**KDE** — System Settings → Keyboard → Shortcuts → Add New → Command: `voice2text` → Shortcut: `Alt+Space`

Check: `echo $XDG_SESSION_TYPE` → `x11` or `wayland` to know which dependency set you need.

---

## Use

1. Focus a text field.
2. Press `Alt+Space` — recording starts (silent).
3. Speak, press `Alt+Space` again — text is pasted via `Ctrl+Shift+V`.

The transcript also stays in your clipboard.

> If a terminal has focus, the text will be typed at the prompt. Focus the destination first.

---

## Uninstall

```bash
rm ~/.local/bin/voice2text
```

Then remove the keybinding from your window manager.
