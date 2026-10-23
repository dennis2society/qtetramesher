# Linux AppImage packaging

Builds a portable `QTetraMesher-<version>-x86_64_Linux.AppImage` in the project root.

## Requirements

- A finished Release build in `build/` (`build/QTetraMesher` must exist):
  ```bash
  cmake -B build -D CMAKE_BUILD_TYPE=Release
  cmake --build build -j
  ```
- Qt 6 with `qmake6` (or `qmake`) on the `PATH`, or set `QMAKE=/path/to/qmake6`
- `wget` and internet access on the first run (downloads `linuxdeploy` and its Qt plugin
  into `build/appimage-tools`)
- The SOFA installation used for the build

## Run

From the project root:

```bash
SOFA_LIBS_PATH=/opt/sofa_git/build/install/lib zz_linux_packaging/build_appimage.sh [version]
```

- `SOFA_LIBS_PATH`: SOFA `lib` directory (default `/opt/sofa_git/build/install/lib`)
- `version` (argument, or `APP_VERSION` environment variable): used in the file name,
  defaults to the project version plus `QTM_VERSION_SUFFIX` in `CMakeLists.txt`

Examples:

```bash
zz_linux_packaging/build_appimage.sh                 # QTetraMesher-0.8.5-x86_64_Linux.AppImage
zz_linux_packaging/build_appimage.sh 1.0.0-rc1       # QTetraMesher-1.0.0-rc1-x86_64_Linux.AppImage
```

## Content

The AppImage contains the executable, the SOFA, Qt 6 and other shared libraries it needs,
and the `data/` example meshes (`usr/share/qtetramesher/data`, used as the start folder
of the file dialogs).

## Notes

- Build on an older distribution for wider compatibility: the AppImage only runs on systems
  with the same or a newer glibc than the build machine.
- OpenGL/graphics driver libraries are not bundled; the host's are used.
- Only the X11 (`xcb`) Qt platform plugin is bundled. For Wayland, set
  `EXTRA_PLATFORM_PLUGINS="libqwayland-generic.so;libqwayland-egl.so"` in the script.
- Missing Qt plugin dependencies (e.g. `libjxrglue.so.0` from `kimageformats`) abort the
  build: install the package that provides them.
- If the AppImage cannot start because FUSE is missing, run it with
  `--appimage-extract-and-run`.
