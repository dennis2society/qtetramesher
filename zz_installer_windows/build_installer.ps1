<#
.SYNOPSIS
  Builds the QTetraMesher Setup.exe with Inno Setup.

.DESCRIPTION
  Refreshes the portable folder (collect_portable.ps1) and compiles
  QTetraMesher.iss. The installer is written to zz_installer_windows\output.
  Requires Inno Setup 6.3+ (https://jrsoftware.org/isdl.php).

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File build_installer.ps1
#>
[CmdletBinding()]
param(
  [string]$Config = 'MinSizeRel',
  [switch]$SkipCollect
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent

$iscc = (Get-Command ISCC.exe -ErrorAction SilentlyContinue).Source
if (-not $iscc) {
  $iscc = @(${env:ProgramFiles(x86)}, $env:ProgramFiles, (Join-Path $env:LOCALAPPDATA 'Programs')) |
    ForEach-Object { Get-ChildItem $_ -Directory -Filter 'Inno Setup*' -ErrorAction SilentlyContinue } |
    Sort-Object Name -Descending |
    ForEach-Object { Join-Path $_.FullName 'ISCC.exe' } |
    Where-Object { Test-Path $_ } | Select-Object -First 1
}
if (-not $iscc) {
  throw 'Inno Setup (ISCC.exe) not found. Install it from https://jrsoftware.org/isdl.php'
}

if (-not $SkipCollect) {
  & (Join-Path $PSScriptRoot 'collect_portable.ps1') -Config $Config -SmokeTest
}

$version = '0.8.5'
$cmake = Get-Content (Join-Path $repoRoot 'CMakeLists.txt') -Raw
if ($cmake -match 'project\(QTetraMesher VERSION ([\d.]+)\)') { $version = $Matches[1] }

& $iscc "/DAppVersion=$version" (Join-Path $PSScriptRoot 'QTetraMesher.iss')
if ($LASTEXITCODE -ne 0) { throw "ISCC failed with exit code $LASTEXITCODE" }
Write-Host "Installer: $(Join-Path $PSScriptRoot "output\QTetraMesher_Setup_$version.exe")"
