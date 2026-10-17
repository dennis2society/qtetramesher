# QTetraMesher Windows packaging

Creates a portable folder and a Setup.exe from a finished build. All commands are run from the repository root in PowerShell.

## Prerequisites

- A successful build of the configuration you want to package:
  ```powershell
  cmake --build build --config MinSizeRel
  ```
- Visual Studio 2022 (provides `dumpbin` and the VC runtime DLLs) and Qt with `windeployqt`. Their locations are read from `build\CMakeCache.txt`.
- For the installer only: [Inno Setup](https://jrsoftware.org/isdl.php) 6.3 or newer.

## 1. Portable app

```powershell
powershell -ExecutionPolicy Bypass -File zz_installer_windows\collect_portable.ps1 -SmokeTest
```

Collects `QTetraMesher.exe`, the project DLLs, Qt DLLs and plugins, SOFA, GMSH, assimp and GMP/MPFR DLLs, the VC runtime, `data/` and the licenses into
`QTetraMesher_Win64_VS2022_0.8.5`. The folder runs by double-clicking `QTetraMesher.exe`; copy it anywhere.

Options:

| Option | Meaning |
| --- | --- |
| `-Config <name>` | Build configuration to package (default `MinSizeRel`). |
| `-Destination <dir>` | Output folder (default `QTetraMesher_Win64_VS2022_0.8.5`). |
| `-Clean` | Empty the output folder first (keeps `library_versions.txt`). |
| `-SmokeTest` | Start the exe with a minimal `PATH` and check that its window opens. |

Check the output for `Unresolved DLLs` warnings; they mean a dependency was not found.

## 2. Installer (Setup.exe)

```powershell
powershell -ExecutionPolicy Bypass -File zz_installer_windows\build_installer.ps1
```

This refreshes the portable folder (including the smoke test) and compiles [QTetraMesher.iss](QTetraMesher.iss).
The result is `zz_installer_windows\output\QTetraMesher_Setup_<version>.exe`; the version is taken from `project(QTetraMesher VERSION ...)` in `CMakeLists.txt`.

Use `-SkipCollect` to compile the installer from the existing portable folder without refreshing it.

The installer copies the app to a chosen folder (per-user by default, all users via the privileges dialog), and offers an optional Start Menu shortcut (checked) and Desktop shortcut (unchecked), both using `resources\qtetramesher.ico`.

## After a rebuild

Rerun step 1 or 2. The portable folder is not updated by the build itself.
