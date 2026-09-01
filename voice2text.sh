#!/bin/sh
AUDIO=/tmp/voice2text.wav
PIDFILE=/tmp/voice2text.pid
API=https://api.groq.com/openai/v1
STT_MODEL=whisper-large-v3-turbo

[ -n "$GROQ_API_KEY" ] || exit 1

if [ -f "$PIDFILE" ]; then
    kill -TERM "$(cat "$PIDFILE")"
    wait "$(cat "$PIDFILE")" 2>/dev/null
    rm -f "$PIDFILE"
else
    rm -f "$PIDFILE"
    arecord -q -f S16_LE -r 16000 -c 1 "$AUDIO" &
    echo $! > "$PIDFILE"
    exit 0
fi

[ -s "$AUDIO" ] || exit 1

TEXT=$(curl -sS --max-time 300 "$API/audio/transcriptions" \
        -H "Authorization: Bearer $GROQ_API_KEY" \
        -F file=@"$AUDIO;type=audio/wav" \
        -F model="$STT_MODEL" \
        -F language=en \
        | jq -r '.text // empty')

rm -f "$AUDIO"
[ -n "$TEXT" ] || exit 1

xdotool type --clearmodifiers "$TEXT"