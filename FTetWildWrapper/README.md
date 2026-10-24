# FTetWildWrapper

Optional QTetraMesher module that offers
[fTetWild](https://github.com/wildmeshing/fTetWild) as a tetrahedralization
method. fTetWild is robust against imperfect input: it also meshes surfaces
with small holes or self-intersections.

The wrapper is a shared library (`FTetWildWrapper.so` / `FTetWildWrapper.dll`)
with the same interface as the other method wrappers: it takes the surface
vertices and triangles in memory and returns the tetrahedra vertices and
indices. No temporary mesh files are written.

## What happens during the build

When the module is enabled (`QTM_WITH_FTETWILD=ON`), the configure step of
QTetraMesher does the following in `FTetWildWrapper/CMakeLists.txt`:

1. **Checks the requirements.** If one is missing, a CMake warning is printed
   and QTetraMesher is built *without* fTetWild (the build does not fail):
   - CMake 3.20 or newer (required by fTetWild),
   - the fTetWild source tree in `FTETWILD_SOURCE_DIR`,
   - GMP (`gmp.h` and the `gmp`/`mpir` library).
2. **Provides Eigen** as the target `Eigen3::Eigen`: an installed Eigen 3.3+
   is used if CMake finds one, otherwise Eigen 3.4.0 is downloaded. This is
   needed because libigl's own Eigen download is rejected by CMake 4. The
   CMake package registry is ignored on purpose, as it may point to Eigen
   build trees of other (possibly broken) builds.
3. **Patches the fTetWild sources** (see below).
4. **Adds the fTetWild sources** with `add_subdirectory()` into
   `<build>/FTetWildWrapper/fTetWild`. fTetWild downloads its dependencies
   with FetchContent into `<build>/_deps` (an internet connection is needed
   on the first configure):
   libigl, geogram, spdlog, fmt, oneTBB and nlohmann json.
   Only the targets needed by the wrapper are built (not fTetWild's command
   line program or tests).
5. **Builds `FTetWildWrapper`**, which links the static fTetWild library and
   its static dependencies (geogram, libigl, spdlog, fmt).
6. **Copies the runtime files** into the build folder, next to the
   QTetraMesher executable:
   - `FTetWildWrapper.so` / `FTetWildWrapper.dll`,
   - TBB (`libtbb.so.12*` on Linux, `tbb12.dll` on Windows; `tbb12_debug`
     in Debug builds),
   - on Windows the GMP/MPIR DLL (`gmp.dll`, `libgmp-10.dll`, `gmp-10.dll`
     or `mpir.dll`, searched next to the GMP import library and in `../bin`;
     can be set explicitly with `GMP_DLL`). On Linux the system GMP is used.

QTetraMesher is then linked against `FTetWildWrapper` and compiled with
`QTM_HAVE_FTETWILD`, which adds "fTetWild Tetrahedralize" to the list of
methods. Without the module the method is simply not offered.

The first build takes a while (geogram and libigl are large). Use a Release
build: in a Debug build fTetWild is many times slower.

## Patch of the fTetWild sources

fTetWild always writes the debug file `<output path>_<postfix>_tracked_surface.stl`
(in `get_tracked_surface()`, `src/MeshImprovement.cpp`). Used as a library
without an output path, this creates `__tracked_surface.stl` in the current
working directory on every run.

The configure step therefore patches `src/MeshImprovement.cpp` in
`FTETWILD_SOURCE_DIR` automatically, so that the file is only written when an
output path is set. fTetWild's own command line program always sets one and
is not affected:

```diff
-    igl::writeSTL(mesh.params.output_path + "_" + mesh.params.postfix + "_tracked_surface.stl", V_sf, F_sf);
+    if (!mesh.params.output_path.empty()) igl::writeSTL(mesh.params.output_path + "_" + mesh.params.postfix + "_tracked_surface.stl", V_sf, F_sf);
```

- The patch is a plain text replacement done by CMake (no `git` or `patch`
  program needed) and works with LF and CRLF line endings.
- It is applied only once; further configure runs see the patched line and
  leave the file alone.
- The configure output shows `Patched .../MeshImprovement.cpp` when the patch
  was applied. If the line was not found (e.g. a newer fTetWild changed it),
  a warning is printed and the build continues; fTetWild then just writes the
  debug file again.
- `git diff` in the fTetWild checkout shows the change;
  `git checkout -- src/MeshImprovement.cpp` reverts it (it is re-applied on
  the next configure).

To apply it by hand, replace the line in `src/MeshImprovement.cpp` as shown
above.

fTetWild also writes `__cutting.stl` when its log level is below 3; the
wrapper uses log level 3, so that file is not created.

## CMake options

| Variable | Default | Meaning |
|---|---|---|
| `QTM_WITH_FTETWILD` | `ON` on Linux, `OFF` on Windows | Build the module |
| `FTETWILD_SOURCE_DIR` | `/opt/ftetwild_git` (Linux), `D:/devel/ftetwild_git` (Windows) | fTetWild git checkout |
| `GMP_INCLUDE_DIRS` | found automatically | Directory containing `gmp.h` |
| `GMP_LIBRARIES` | found automatically | GMP/MPIR library (import library on Windows) |
| `GMP_DLL` | found automatically (Windows only) | GMP/MPIR DLL copied to the build folder |

The environment variables `GMP_INC` and `GMP_LIB` are used as additional
search paths for GMP.

## Linux

Install GMP and (optionally) Eigen, e.g. on Arch `pacman -S gmp eigen`, on
Debian/Ubuntu `apt install libgmp-dev libeigen3-dev`.

Get fTetWild:

```sh
git clone https://github.com/wildmeshing/fTetWild.git /opt/ftetwild_git
```

Configure and build QTetraMesher as usual; the module is enabled by default:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
      -DFTETWILD_SOURCE_DIR=/opt/ftetwild_git
cmake --build build -j$(nproc)
```

Disable the module:

```sh
cmake -S . -B build -DQTM_WITH_FTETWILD=OFF
```

## Windows

The module now works fine on Windows and therefore enabled by default in the CI bulild.

1. Clone fTetWild, e.g. to `D:/devel/ftetwild_git`.
2. Get GMP for MSVC, e.g. with vcpkg (`vcpkg install gmp:x64-windows`) or
   MPIR. You need `gmp.h`, the import library (`gmp.lib`/`mpir.lib`) and the
   DLL.
3. Optionally get Eigen (e.g. `vcpkg install eigen3:x64-windows`), otherwise
   it is downloaded.
4. Configure with the module enabled:

```bat
cmake -S . -B build -G "Visual Studio 17 2022" ^
      -DQTM_WITH_FTETWILD=ON ^
      -DFTETWILD_SOURCE_DIR=D:/devel/ftetwild_git ^
      -DGMP_INCLUDE_DIRS=D:/devel/vcpkg/installed/x64-windows/include ^
      -DGMP_LIBRARIES=D:/devel/vcpkg/installed/x64-windows/lib/gmp.lib
cmake --build build --config Release
```

With vcpkg you can instead pass the toolchain file
(`-DCMAKE_TOOLCHAIN_FILE=D:/devel/vcpkg/scripts/buildsystems/vcpkg.cmake`),
then GMP and Eigen are found automatically.

If the configure step warns that the GMP DLL was not found, set `GMP_DLL` to
the DLL, or copy it next to `QTetraMesher.exe` yourself. If fTetWild does not
compile with your compiler, reconfigure with `-DQTM_WITH_FTETWILD=OFF`; the
rest of QTetraMesher is not affected.

The portable package script (`zz_installer_windows/collect_portable.ps1`)
picks up `FTetWildWrapper.dll`, `tbb12.dll` and the GMP DLL automatically,
because it copies the DLLs from the build folder and resolves the remaining
dependencies.
