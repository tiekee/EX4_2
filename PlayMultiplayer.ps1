param([string]$Engine = 'C:\Program Files\Epic Games\UE_5.6')
$projectFile = Join-Path $PSScriptRoot 'EX4_2.uproject'
$editorFile = Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor.exe'
if (!(Test-Path -LiteralPath $editorFile)) { throw 'UnrealEditor.exe not found. Set the Engine parameter.' }
$quotedProject = '"' + $projectFile + '"'
Start-Process -FilePath $editorFile -ArgumentList @($quotedProject,'/Game/VehicleTemplate/Maps/VehicleAdvExampleMap?listen','-port=7781','-game','-windowed','-ResX=1100','-ResY=720','-WinX=20','-WinY=20')
Start-Sleep -Seconds 10
Start-Process -FilePath $editorFile -ArgumentList @($quotedProject,'127.0.0.1:7781','-game','-windowed','-ResX=1100','-ResY=720','-WinX=450','-WinY=70')
