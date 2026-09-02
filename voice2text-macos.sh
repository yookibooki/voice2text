#!/bin/sh
umask 077
O="/tmp/v2t.wav"
API="https://api.groq.com/openai/v1/audio/transcriptions"
if pkill -2 -f "swift.*$O" 2>/dev/null; then
while pgrep -f "swift.*$O" >/dev/null; do sleep 0.05; done
[ -s "$O" ] || exit 1
T=$(curl -sS "$API" -H "Authorization: Bearer $GROQ_API_KEY" -F "file=@$O;type=audio/wav" -F model=whisper-large-v3-turbo | python3 -c 'import json,sys;print(json.load(sys.stdin).get("text",""))')
rm -f "$O"
[ "$T" ] || exit 0
printf '%s' "$T" | pbcopy
osascript -e 'tell application "System Events" to keystroke "v" using command down'
exit 0
fi
swift -e 'import AVFoundation;let u=URL(fileURLWithPath:CommandLine.arguments[1]);let s:[String:Any]=[AVFormatIDKey:Int(kAudioFormatLinearPCM),AVSampleRateKey:16000,AVNumberOfChannelsKey:1,AVLinearPCMBitDepthKey:16,AVLinearPCMIsFloatKey:false,AVLinearPCMIsNonInterleaved:false];let r=try! AVAudioRecorder(url:u,settings:s);r.record();RunLoop.main.run()' "$O" &
