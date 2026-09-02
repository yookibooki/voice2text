#!/bin/sh
umask 077
[ "$GROQ_API_KEY" ] || exit 1

DIR="${XDG_RUNTIME_DIR:-/tmp/v2t-$(id -u)}"
mkdir -p -m 700 "$DIR" 2>/dev/null
O="$DIR/v2t.ogg"
API="https://api.groq.com/openai/v1/audio/transcriptions"

if pkill -2 -f "arecord -q -t raw -f S16_LE -r 48000" 2>/dev/null; then
    while pgrep -f "opusenc.*$O" >/dev/null; do sleep 0.05; done

    [ -s "$O" ] || exit 1
    [ "$(wc -c <"$O" | tr -d ' ')" -lt 3000 ] && rm -f "$O" && exit 0
    T=$(curl -sS "$API" -H "Authorization: Bearer $GROQ_API_KEY" \
        -F "file=@$O;type=audio/ogg" -F model="${V2T_MODEL:-whisper-large-v3-turbo}" -F language="${V2T_LANG:-en}" -F temperature=0 | jq -r '.text // empty')
    rm -f "$O"

    [ "$T" ] || exit 0
    if [ "$WAYLAND_DISPLAY" ]; then
        printf '%s' "$T" | wl-copy
        sleep 0.15
        wtype -M ctrl -M shift -P v -p v -m shift -m ctrl 2>/dev/null || wtype -M ctrl -P v -p v -m ctrl 2>/dev/null
    else
        printf '%s' "$T" | xclip -selection clipboard
        sleep 0.15
        xdotool key --clearmodifiers ctrl+shift+v 2>/dev/null || xdotool key --clearmodifiers ctrl+v
    fi
    exit 0
fi

arecord -q -t raw -f S16_LE -r 48000 -c 1 | opusenc --quiet --raw --raw-bits 16 --raw-rate 48000 --raw-chan 1 --speech - "$O" &
