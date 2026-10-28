param([string]$Engine = 'C:\Program Files\Epic Games\UE_5.6')
$projectFile = Join-Path $PSScriptRoot 'EX4_2.uproject'
$buildFile = Join-Path $Engine 'Engine\Build\BatchFiles\Build.bat'
& $buildFile EX4_2Editor Win64 Development "-Project=$projectFile" -WaitMutex -NoHotReloadFromIDE -NoUBA -NoUbaLocal -NoUBTMakefiles
exit $LASTEXITCODE
