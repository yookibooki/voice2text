#!/bin/sh
umask 077
O="${XDG_RUNTIME_DIR:-/tmp}/v2t.ogg"
API="https://api.groq.com/openai/v1/audio/transcriptions"

if pkill -2 -f "arecord -q -t raw" 2>/dev/null; then
    while pgrep -f "opusenc.*$O" >/dev/null; do sleep 0.05; done
    [ -s "$O" ] || exit 1
    T=$(curl -sS "$API" -H "Authorization: Bearer $GROQ_API_KEY" -F "file=@$O;type=audio/ogg" -F model=whisper-large-v3-turbo | jq -r '.text // empty')
    rm -f "$O"

    [ "$T" ] || exit 0
    if [ "$WAYLAND_DISPLAY" ]; then
        printf '%s' "$T" | wl-copy
        sleep 0.15
        wtype -M ctrl -M shift v 2>/dev/null || wtype -M ctrl v 2>/dev/null
    else
        printf '%s' "$T" | xclip -selection clipboard
        sleep 0.15
        xdotool key --clearmodifiers ctrl+shift+v 2>/dev/null || xdotool key --clearmodifiers ctrl+v
    fi
    exit 0
fi
arecord -q -t raw -f S16_LE -r 16000 -c 1 | opusenc --quiet --raw --raw-bits 16 --raw-rate 16000 --raw-chan 1 - "$O" &
