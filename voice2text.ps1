$O="$env:TEMP\v2t.wav"
$F="$env:TEMP\v2t.rec"
$API="https://api.groq.com/openai/v1/audio/transcriptions"
Add-Type @"
using System.Runtime.InteropServices;
public class MCI{ [DllImport("winmm.dll")] public static extern int mciSendString(string c, System.Text.StringBuilder r, int l, int n);}
"@
if (Test-Path $F) {
[MCI]::mciSendString("stop rec",$null,0,0)|Out-Null
[MCI]::mciSendString("save rec $O",$null,0,0)|Out-Null
[MCI]::mciSendString("close rec",$null,0,0)|Out-Null
if (!(Test-Path $O) -or (Get-Item $O).Length -lt 3000) {Remove-Item $O,$F -Force -ErrorAction SilentlyContinue; exit 1}
$M=$env:V2T_MODEL; if(-not $M){$M="whisper-large-v3-turbo"}; $L=$env:V2T_LANG; if(-not $L){$L="en"}
$T=(curl.exe -sS $API -H "Authorization: Bearer $env:GROQ_API_KEY" -F "file=@$O;type=audio/wav" -F "model=$M" -F "language=$L" -F "temperature=0" | ConvertFrom-Json).text
Remove-Item $O,$F -Force
Set-Clipboard $T
Start-Sleep -Milliseconds 150
Add-Type -AssemblyName System.Windows.Forms
[System.Windows.Forms.SendKeys]::SendWait("^v")
exit 0
}
[MCI]::mciSendString("open new Type waveaudio alias rec",$null,0,0)|Out-Null
[MCI]::mciSendString("set rec time format ms bitspersample 16 channels 1 samplespersec 16000 bytespersec 32000 alignment 2",$null,0,0)|Out-Null
[MCI]::mciSendString("record rec",$null,0,0)|Out-Null
New-Item -Path $F -ItemType File -Force|Out-Null
