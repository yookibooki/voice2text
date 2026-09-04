#!/bin/sh
umask 077
read -r GROQ_API_KEY 2>/dev/null <"${XDG_CONFIG_HOME:-$HOME/.config}/voice2text/groq.key"
[ "$GROQ_API_KEY" ] || exit 1
O="${TMPDIR:-/tmp}/v2t.wav"
API="https://api.groq.com/openai/v1/audio/transcriptions"
if pkill -2 -f "swift.*$O" 2>/dev/null; then
while pgrep -f "swift.*$O" >/dev/null; do sleep 0.05; done
[ -s "$O" ] || exit 1
S=$(wc -c <"$O" | tr -d ' '); [ "$S" -lt 3000 ] && rm -f "$O" && exit 0
T=$(curl -sS "$API" -H "Authorization: Bearer $GROQ_API_KEY" -F "file=@$O;type=audio/wav" -F model="${V2T_MODEL:-whisper-large-v3-turbo}" -F language="${V2T_LANG:-en}" -F temperature=0 | python3 -c 'import json,sys;print(json.load(sys.stdin).get("text",""))')
rm -f "$O"
[ "$T" ] || exit 0
printf '%s' "$T" | pbcopy
osascript -e 'tell application "System Events" to keystroke "v" using command down'
exit 0
fi
swift -e 'import AVFoundation;import Darwin;let u=URL(fileURLWithPath:CommandLine.arguments[1]);let s:[String:Any]=[AVFormatIDKey:Int(kAudioFormatLinearPCM),AVSampleRateKey:16000,AVNumberOfChannelsKey:1,AVLinearPCMBitDepthKey:16,AVLinearPCMIsFloatKey:false,AVLinearPCMIsNonInterleaved:false];let r=try! AVAudioRecorder(url:u,settings:s);signal(SIGINT){_ in r.stop();exit(0)};r.record();RunLoop.main.run()' "$O" &
