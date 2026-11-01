<#
.SYNOPSIS
  Collects QTetraMesher and all its runtime dependencies into one portable folder.

.DESCRIPTION
  Copies QTetraMesher.exe and the project DLLs from the CMake build tree, lets
  windeployqt add the Qt DLLs and plugins, then walks the PE imports of every
  collected file (dumpbin) and copies the missing DLLs from the dependency
  install folders (SOFA, GMSH, assimp, GMP/MPFR, oneTBB for fTetWild, VC
  runtime). Dependency locations are read from build\CMakeCache.txt.

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File collect_portable.ps1 -SmokeTest

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File collect_portable.ps1 -Version 0.8.6 -Suffix _rc1 -Zip
#>
[CmdletBinding()]
param(
  [string]$Config = 'MinSizeRel',
  [string]$BuildDir,
  [string]$Destination,
  # default: project(QTetraMesher VERSION ...) in CMakeLists.txt
  [string]$Version,
  # appended to the version in folder and file names, e.g. _rc1
  [string]$Suffix = '',
  [switch]$Clean,
  [switch]$SmokeTest,
  # also writes zz_installer_windows\QTetraMesher_Win64_<version><suffix>_portable.zip
  [switch]$Zip
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
# both end up in file names
if ($Version -notmatch '^[A-Za-z0-9._-]+$') { throw "Invalid version '$Version'" }
if ($Suffix -and $Suffix -notmatch '^[A-Za-z0-9._-]+$') { throw "Invalid suffix '$Suffix'" }
$name = "$Version$Suffix"
if (-not $BuildDir)    { $BuildDir    = Join-Path $repoRoot 'build' }
if (-not $Destination) { $Destination = Join-Path $repoRoot "QTetraMesher_Win64_VS2022_$name" }

# ---------------------------------------------------------------- build tree
$cacheFile = Join-Path $BuildDir 'CMakeCache.txt'
$exeSource = Join-Path (Join-Path $BuildDir $Config) 'QTetraMesher.exe'
if (-not (Test-Path $cacheFile)) { throw "CMakeCache.txt not found in $BuildDir" }
if (-not (Test-Path $exeSource)) {
  throw "$exeSource not found. Build first: cmake --build build --config $Config"
}

function Get-CacheValue([string]$Name) {
  $m = Select-String -Path $cacheFile -Pattern ('^' + [regex]::Escape($Name) + ':[A-Z]+=(.*)$') |
    Select-Object -First 1
  if ($m) { return $m.Matches[0].Groups[1].Value.Trim() }
  return $null
}

function Get-ParentDir([string]$Path, [int]$Levels = 1) {
  for ($i = 0; $i -lt $Levels; $i++) { $Path = Split-Path $Path -Parent }
  return $Path
}

# ------------------------------------------------------------ search folders
$searchDirs = New-Object System.Collections.Generic.List[string]
function Add-SearchDir([string]$Path) {
  if ($Path -and (Test-Path $Path -PathType Container)) {
    $searchDirs.Add((Resolve-Path $Path).Path)
  } elseif ($Path) {
    Write-Warning "Search folder not found: $Path"
  }
}

# project DLLs of the selected configuration
Get-ChildItem $BuildDir -Directory | ForEach-Object {
  $d = Join-Path $_.FullName $Config
  if (Test-Path $d) { Add-SearchDir $d }
}

$qt6Dir = Get-CacheValue 'Qt6_DIR'
if (-not $qt6Dir) { throw 'Qt6_DIR not found in CMakeCache.txt' }
$qtRoot = Get-ParentDir $qt6Dir 3
Add-SearchDir (Join-Path $qtRoot 'bin')

$sofaDir = Get-CacheValue 'SOFA_INSTALL_DIR'
Add-SearchDir (Join-Path $sofaDir 'bin')

$gmshLib = Get-CacheValue 'GMSH_LIBRARY'
if ($gmshLib) {
  Add-SearchDir (Get-ParentDir $gmshLib 1)
  Add-SearchDir (Join-Path (Get-ParentDir $gmshLib 2) 'bin')
}

# <prefix>\share\assimp -> <prefix>\bin (vcpkg: assimp, GMP, MPFR, zlib, ...)
$assimpDir = Get-CacheValue 'assimp_DIR'
if ($assimpDir) { Add-SearchDir (Join-Path (Get-ParentDir $assimpDir 2) 'bin') }

foreach ($libVar in 'GMP_LIBRARIES', 'GMP_LIBRARY_RELEASE') {
  $gmpLib = Get-CacheValue $libVar
  if ($gmpLib) { Add-SearchDir (Join-Path (Get-ParentDir $gmpLib 2) 'bin') }
}

foreach ($prefix in ((Get-CacheValue 'CMAKE_PREFIX_PATH') -split ';')) {
  if ($prefix) { Add-SearchDir (Join-Path $prefix 'bin') }
}

# oneTBB (fetched by fTetWild) writes tbb12.dll to build\msvc_*_<config in lower case>
Get-ChildItem $BuildDir -Directory -Filter "msvc_*_$($Config.ToLowerInvariant())" | ForEach-Object { Add-SearchDir $_.FullName }

# ------------------------------------------------------------------- VS tools
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path $vswhere)) { throw 'vswhere.exe not found (Visual Studio 2022 required)' }
$vsPath = & $vswhere -latest -products * -property installationPath | Select-Object -First 1
if (-not $vsPath) { throw 'Visual Studio installation not found' }

$dumpbin = Get-ChildItem (Join-Path $vsPath 'VC\Tools\MSVC') -Recurse -Filter dumpbin.exe |
  Where-Object { $_.FullName -match '\\Hostx64\\x64\\' } |
  Sort-Object FullName -Descending | Select-Object -First 1 -ExpandProperty FullName
if (-not $dumpbin) { throw 'dumpbin.exe (x64) not found' }

$redistVersion = Get-ChildItem (Join-Path $vsPath 'VC\Redist\MSVC') -Directory |
  Where-Object { $_.Name -match '^\d+\.\d+' } |
  Sort-Object { [version]$_.Name } -Descending | Select-Object -First 1
if (-not $redistVersion) { throw 'VC redistributable folder not found' }
$redistDirs = Get-ChildItem (Join-Path $redistVersion.FullName 'x64') -Directory -Filter 'Microsoft.VC*' |
  Where-Object { $_.Name -notmatch 'Debug' } | ForEach-Object { $_.FullName }

$windeployqt = Join-Path $qtRoot 'bin\windeployqt.exe'
if (-not (Test-Path $windeployqt)) { throw "windeployqt.exe not found in $qtRoot\bin" }

# ---------------------------------------------------------------- destination
if ($Clean -and (Test-Path $Destination)) {
  # keep the hand-written version list
  Get-ChildItem $Destination -Force |
    Where-Object { $_.Name -ne 'library_versions.txt' } |
    Remove-Item -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $Destination | Out-Null

Write-Host "Build config : $Config"
Write-Host "Destination  : $Destination"
Write-Host 'Search folders:'
$searchDirs | ForEach-Object { Write-Host "  $_" }

# ------------------------------------------------------- application + Qt
Copy-Item $exeSource $Destination -Force
foreach ($d in $searchDirs) {
  if ($d -like (Join-Path $BuildDir '*')) {
    Get-ChildItem $d -Filter *.dll | Copy-Item -Destination $Destination -Force
  }
}

Write-Host 'Running windeployqt...'
& $windeployqt --release --no-translations --no-compiler-runtime --no-opengl-sw `
  --dir $Destination (Join-Path $Destination 'QTetraMesher.exe') | Out-Null
if ($LASTEXITCODE -ne 0) { throw "windeployqt failed with exit code $LASTEXITCODE" }

# -------------------------------------------------- remaining DLL dependencies
$vcRuntime = '^(vcruntime|msvcp|concrt|vcomp|vccorlib)\d+.*\.dll$'
$system32  = Join-Path $env:windir 'System32'
$seen      = @{}
$missing   = New-Object System.Collections.Generic.HashSet[string]

function Get-Imports([string]$File) {
  & $dumpbin /nologo /dependents $File | ForEach-Object {
    if ($_ -match '^\s+(\S+\.dll)\s*$') { $Matches[1] }
  }
}

# Returns $true if a file was copied into the destination.
function Resolve-Dependency([string]$Name) {
  $key = $Name.ToLowerInvariant()
  if ($seen.ContainsKey($key)) { return $false }
  $seen[$key] = $true

  if (Test-Path (Join-Path $Destination $Name)) { return $false }
  if ($key -match '^(api|ext)-ms-') { return $false }

  $candidates = if ($key -match $vcRuntime) { $redistDirs }
                elseif (Test-Path (Join-Path $system32 $Name)) { return $false }
                else { $searchDirs }

  foreach ($dir in $candidates) {
    $p = Join-Path $dir $Name
    if (Test-Path $p) {
      Copy-Item $p $Destination -Force
      Write-Host "  + $Name  <- $dir"
      return $true
    }
  }
  [void]$missing.Add($Name)
  return $false
}

Write-Host 'Resolving DLL dependencies...'
$checked = @{}
do {
  $copied = $false
  $files = Get-ChildItem $Destination -Recurse -File |
    Where-Object { $_.Extension -in '.dll', '.exe' }
  foreach ($f in $files) {
    if ($checked.ContainsKey($f.FullName)) { continue }
    $checked[$f.FullName] = $true
    foreach ($dep in (Get-Imports $f.FullName)) {
      if (Resolve-Dependency $dep) { $copied = $true }
    }
  }
} while ($copied)

if ($missing.Count -gt 0) {
  $msg = 'Unresolved DLLs (not found in any search folder): ' + (($missing | Sort-Object) -join ', ')
  if ($env:GITHUB_ACTIONS) { Write-Host "::warning::$msg" }
  Write-Warning $msg
}

# ------------------------------------------------------ resources + licenses
$dataSource = Join-Path $repoRoot 'data'
if (Test-Path $dataSource) {
  Copy-Item $dataSource $Destination -Recurse -Force
}
foreach ($resourceName in 'gpl.txt', 'lgpl-2.1.txt', 'README.md') {
  Copy-Item (Join-Path $repoRoot $resourceName) $Destination -Force
}
$tetgenLicense = Join-Path $repoRoot 'TetgenWrapper\LICENSE'
if (Test-Path $tetgenLicense) {
  New-Item -ItemType Directory -Force -Path (Join-Path $Destination 'licenses') | Out-Null
  Copy-Item $tetgenLicense (Join-Path $Destination 'licenses\TetGen_LICENSE.txt') -Force
}
$ftetwildDir = Get-CacheValue 'FTETWILD_SOURCE_DIR'
$ftetwildLicense = if ($ftetwildDir) { Join-Path $ftetwildDir 'LICENSE.MPL2' }
if ($ftetwildLicense -and (Test-Path $ftetwildLicense)) {
  New-Item -ItemType Directory -Force -Path (Join-Path $Destination 'licenses') | Out-Null
  Copy-Item $ftetwildLicense (Join-Path $Destination 'licenses\fTetWild_LICENSE.MPL2.txt') -Force
}
$geogramLicense = Join-Path $BuildDir '_deps\geogram-src\LICENSE'
if (Test-Path $geogramLicense) {
  New-Item -ItemType Directory -Force -Path (Join-Path $Destination 'licenses') | Out-Null
  Copy-Item $geogramLicense (Join-Path $Destination 'licenses\geogram_LICENSE.txt') -Force
}

$size = (Get-ChildItem $Destination -Recurse -File | Measure-Object Length -Sum).Sum / 1MB
Write-Host ('Portable folder ready: {0:N1} MB' -f $size)

# ----------------------------------------------------------------- smoke test
if ($SmokeTest) {
  Write-Host 'Smoke test: starting the exe with a minimal PATH...'
  $oldPath = $env:PATH
  $env:PATH = "$system32;$env:windir"
  $proc = $null
  try {
    $proc = Start-Process (Join-Path $Destination 'QTetraMesher.exe') `
      -WorkingDirectory $Destination -PassThru
    $ok = $false
    for ($i = 0; $i -lt 30 -and -not $proc.HasExited; $i++) {
      Start-Sleep -Milliseconds 500
      $proc.Refresh()
      if ($proc.MainWindowHandle -ne [IntPtr]::Zero) { $ok = $true; break }
    }
    if (-not $ok) { throw 'QTetraMesher.exe did not open a window (missing DLL or plugin?)' }
    Write-Host 'Smoke test passed.'
  } finally {
    $env:PATH = $oldPath
    if ($proc -and -not $proc.HasExited) {
      [void]$proc.CloseMainWindow()
      if (-not $proc.WaitForExit(3000)) { $proc.Kill() }
    }
  }
}

# ------------------------------------------------------------------------ zip
if ($Zip) {
  $zipFile = Join-Path $PSScriptRoot "QTetraMesher_Win64_${name}_portable.zip"
  if (Test-Path $zipFile) { Remove-Item $zipFile -Force }
  Compress-Archive -Path $Destination -DestinationPath $zipFile
  Write-Host "Portable zip: $zipFile"
}
