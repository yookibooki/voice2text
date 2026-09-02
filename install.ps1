$B="$env:USERPROFILE\bin"
mkdir $B -Force|Out-Null
Copy-Item $PSScriptRoot\voice2text.ps1 $B\voice2text.ps1 -Force
$P=[Environment]::GetEnvironmentVariable("Path","User")
if ($P -notlike "*$B*") {setx PATH "$P;$B"|Out-Null; $env:PATH="$P;$B"}
if (!$env:GROQ_API_KEY) {$K=Read-Host "GROQ_API_KEY"; setx GROQ_API_KEY $K|Out-Null}
$W=New-Object -COM WScript.Shell
$S=$W.CreateShortcut("$env:APPDATA\Microsoft\Windows\Start Menu\Programs\voice2text.lnk")
$S.TargetPath="powershell.exe"
$S.Arguments="-WindowStyle Hidden -File $B\voice2text.ps1"
$S.Hotkey="Alt+Space"
$S.Save()
