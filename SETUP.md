# Setting up Alt+Space

`voice2text` is a toggle — press once to record, press again to stop and transcribe. To make it useful, bind Alt+Space to run `voice2text` in your desktop environment.

Pick your environment below.

---

## i3 / i3wm

Add to `~/.config/i3/config`:

```
bindsym Mod1+space exec --no-startup-id voice2text
```

Reload config: `$mod+Shift+R`

---

## Sway

Add to `~/.config/sway/config`:

```
bindsym Mod1+space exec voice2text
```

Reload config: `$mod+Shift+C`

---

## Hyprland

Add to `~/.config/hypr/hyprland.conf`:

```
bind = ALT, SPACE, exec, voice2text
```

Reload: `hyprctl reload`

---

## GNOME / GNOME on Wayland

```bash
# Create the custom keybinding
gsettings set org.gnome.settings-daemon.plugins.media-keys \
  custom-keybindings "['/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom0/']"

# Set name, command, and binding
gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybindings:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom0/ \
  name "voice2text"

gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybindings:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom0/ \
  command "voice2text"

gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybindings:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom0/ \
  binding "<Alt>space"
```

Or use **Settings → Keyboard → Keyboard Shortcuts → Add Custom Shortcut**.

---

## KDE Plasma

```bash
kwriteconfig5 --file ~/.config/khotkeysrc \
  --group Data --group 1 --group Triggers --group 0 \
  --key Key '<Alt>space'

kwriteconfig5 --file ~/.config/khotkeysrc \
  --group Data --group 1 --group Actions --group 0 \
  --key CommandURL 'voice2text'

kwriteconfig5 --file ~/.config/khotkeysrc \
  --group Data --group 1 --group Actions --group 0 \
  --key Type 'COMMAND'
```

Or **System Settings → Shortcuts → Custom Shortcuts → New → Global Shortcut → Command/URL**.

---

## XFCE

```bash
xfconf-query -c xfce4-keyboard-shortcuts \
  -p '/commands/custom/<Alt>space' \
  -n -t string -s 'voice2text'
```

Or **Settings → Keyboard → Application Shortcuts → Add**.

---

## Cinnamon

```bash
gsettings set org.cinnamon.settings-daemon.plugins.media-keys.custom-keybindings \
  "['/org/cinnamon/settings-daemon/plugins/media-keys/custom-keybindings/custom0/']"

gsettings set org.cinnamon.settings-daemon.plugins.media-keys.custom-keybindings:/org/cinnamon/settings-daemon/plugins/media-keys/custom-keybindings/custom0/ \
  name "voice2text"

gsettings set org.cinnamon.settings-daemon.plugins.media-keys.custom-keybindings:/org/cinnamon/settings-daemon/plugins/media-keys/custom-keybindings/custom0/ \
  command "voice2text"

gsettings set org.cinnamon.settings-daemon.plugins.media-keys.custom-keybindings:/org/cinnamon/settings-daemon/plugins/media-keys/custom-keybindings/custom0/ \
  binding "<Alt>space"
```

---

## MATE

```bash
gsettings set org.mate.Marco.global-keybindings custom-keybindings \
  "['/org/mate/Marco/global-keybindings/custom-keybindings/custom0/']"

gsettings set org.mate.Marco.global-keybindings.custom-keybindings:/org/mate/Marco/global-keybindings/custom-keybindings/custom0/ \
  name "voice2text"

gsettings set org.mate.Marco.global-keybindings.custom-keybindings:/org/mate/Marco/global-keybindings/custom-keybindings/custom0/ \
  action "voice2text"

gsettings set org.mate.Marco.global-keybindings.custom-keybindings:/org/mate/Marco/global-keybindings/custom-keybindings/custom0/ \
  binding "<Alt>space"
```

---

## Budgie

Same as GNOME — use either the `gsettings` commands above or **Budgie Desktop Settings → Keyboard Shortcuts**.

---

## LXQt

**Preferences → LXQt Settings → Shortcut Keys**. Add a new shortcut with `Alt+Space` → `voice2text`.

Or edit `~/.config/lxqt/globalkeyshortcuts.conf`:

```
[D override]
voice2text=Alt+Space
[Command]
voice2text__=voice2text
```

---

## Openbox

Add to `~/.config/openbox/rc.xml` inside the `<keyboard>` section:

```xml
<keybind key="A-space">
  <action name="Execute">
    <command>voice2text</command>
  </action>
</keybind>
```

Reconfigure: `openbox --reconfigure`

---

## AwesomeWM

Add to `~/.config/awesome/rc.lua`:

```lua
awful.key({ "Mod1" }, "space", function()
  awful.spawn("voice2text")
end)
```

---

## DWM / dwm

Edit `config.def.h` (or `config.h`), add a keybinding:

```c
{ MODKEY|Mod1Mask, XK_space, spawn, SHCMD("voice2text") },
```

Recompile and restart dwm.

---

## bspwm

Add to `~/.config/bspwm/bspwmrc`:

```bash
bspc keybind Alt+space "voice2text"
```

Or use `sxhkd` (see below).

---

## sxhkd (common with bspwm, herbstluftwm, etc.)

Add to `~/.config/sxhkd/sxhkdrc`:

```
alt + space
	voice2text
```

---

## herbstluftwm

Add to `~/.config/herbstluftwm/autostart`:

```bash
herbstclient keybind Alt+space spawn voice2text
```

---

## Qtile

Add to `~/.config/qtile/config.py`:

```python
Key([mod], "space", lazy.spawn("voice2text")),
```

---

## spectrwm

Add to `~/.config/spectrwm/spectrwm.conf`:

```
binding[mod+space] = spawn:voice2text
```

---

## Xmonad

In `~/.xmonad/xmonad.hs`:

```haskell
, ((mod1Mask, xK_space), spawn "voice2text")
```

---

## Enlightenment

**Settings → Key Bindings → Add**. Set key to `Alt+Space`, action to `Run Command`, command to `voice2text`.

---

## Deepin

**Settings → Keyboard → Shortcuts → Custom Shortcuts → Add**. Set name `voice2text`, command `voice2text`, shortcut `Alt+Space`.

---

## Generic X11 (any WM)

### Option A: xbindkeys

Install `xbindkeys`, then create `~/.xbindkeysrc`:

```
"voice2text"
    Alt+space
```

Add `xbindkeys` to your `~/.xinitrc` or autostart.

### Option B: .xinitrc / autostart

If your window manager has no hotkey support, use `xdotool` in a loop (not recommended — this is a poll loop):

```bash
xbindkeys -f ~/.xbindkeysrc
```

Or run `xbindkeys` from your WM's autostart.

---

## Wayland fallback

Most Wayland compositors don't expose global keybinding registration. Use your compositor's native config:
- **GNOME on Wayland** → GNOME section above (Settings → Keyboard works on Wayland)
- **KDE on Wayland** → KDE section above (System Settings works on Wayland)
- **Sway** → Sway section above
- **Hyprland** → Hyprland section above

If your compositor isn't listed, check its docs for global shortcut support.
