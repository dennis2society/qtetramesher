<#
.SYNOPSIS
  Builds the QTetraMesher Setup.exe with Inno Setup.

.DESCRIPTION
  Refreshes the portable folder (collect_portable.ps1) and compiles
  QTetraMesher.iss. The installer is written to
  zz_installer_windows\QTetraMesher_Setup_<version><suffix>.exe.
  Requires Inno Setup 6.3+ (https://jrsoftware.org/isdl.php).

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File build_installer.ps1

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File build_installer.ps1 -Version 0.8.6 -Suffix _rc1
#>
[CmdletBinding()]
param(
  [string]$Config = 'MinSizeRel',
  # default: project(QTetraMesher VERSION ...) in CMakeLists.txt
  [string]$Version,
  # appended to the version in file names, e.g. _rc1
  [string]$Suffix = '',
  [switch]$SkipCollect
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent
if (-not $Version) {
  $cmake = Get-Content (Join-Path $repoRoot 'CMakeLists.txt') -Raw
  if ($cmake -notmatch 'project\(QTetraMesher VERSION ([\d.]+)\)') {
    throw 'Version not found in CMakeLists.txt'
  }
  $Version = $Matches[1]
  if ($cmake -match 'set\(QTM_VERSION_SUFFIX\s+"([^"]*)"\)') { $Version += $Matches[1] }
}
if ($Version -notmatch '^[A-Za-z0-9._-]+$') { throw "Invalid version '$Version'" }
if ($Suffix -and $Suffix -notmatch '^[A-Za-z0-9._-]+$') { throw "Invalid suffix '$Suffix'" }
$name = "$Version$Suffix"
# the Windows file version must be numeric, e.g. 0.8.6 for 0.8.6-rc1
$fileVersion = if ($Version -match '^(\d+(\.\d+){0,3})') { $Matches[1] } else { '0.0.0' }

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
  & (Join-Path $PSScriptRoot 'collect_portable.ps1') -Config $Config -Version $Version `
    -Suffix $Suffix -SmokeTest
}

$sourceDir = Join-Path $repoRoot "QTetraMesher_Win64_VS2022_$name"
if (-not (Test-Path $sourceDir)) {
  throw "Portable folder $sourceDir not found (run collect_portable.ps1 with the same -Version/-Suffix)"
}

& $iscc "/DAppVersion=$Version" "/DFileVersion=$fileVersion" "/DNameSuffix=$Suffix" `
  "/DSourceDir=$sourceDir" (Join-Path $PSScriptRoot 'QTetraMesher.iss')
if ($LASTEXITCODE -ne 0) { throw "ISCC failed with exit code $LASTEXITCODE" }
Write-Host "Installer: $(Join-Path $PSScriptRoot "QTetraMesher_Setup_$name.exe")"
