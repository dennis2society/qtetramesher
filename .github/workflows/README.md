# GitHub Actions workflows

| Workflow | File | Runner | Output |
| --- | --- | --- | --- |
| Windows build | [windows.yml](windows.yml) | `windows-2022` | Portable zip and Inno Setup installer |
| Linux AppImage | [linux-appimage.yml](linux-appimage.yml) | `ubuntu-22.04` | `QTetraMesher-<version>-x86_64_Linux.AppImage` |

Both workflows build QTetraMesher from source with fTetWild enabled, package it with the
scripts in `zz_installer_windows/` and `zz_linux_packaging/`, and upload the result as a
build artifact. When started by a tag they also attach the files to the GitHub release.

## Triggers

- **Manual:** Actions tab, select the workflow, **Run workflow**. Only the artifact is
  produced; no release is touched. The workflow file must exist on the default branch for
  the button to appear.
- **Tag push:** tags starting with `v` or a digit (`v0.8.6`, `0.8.6`). The files are also
  attached to the release of that tag (created if it does not exist).

## Dependencies

Nothing is installed on the runner by hand; all locations are passed to CMake by the
workflow, so no manual path setup is needed.

| Dependency | Windows | Linux |
| --- | --- | --- |
| Qt 6 | `install-qt-action` (6.12) | `install-qt-action` (6.8) |
| CGAL, assimp, Eigen, GMP, Boost | vcpkg (`x64-windows`) | apt (Ubuntu 22.04) |
| SOFA | release zip `SOFA_v<ver>_Win64.zip` | release zip `SOFA_v<ver>_Linux_Python3.10.zip` |
| GMSH | built from source with MSVC (the SDK is MinGW) | SDK tgz from gmsh.info |
| fTetWild | git checkout of `wildmeshing/fTetWild` (default branch) | same |

Versions are set in the `env:` block at the top of each workflow (`SOFA_VERSION`,
`GMSH_VERSION`). To update a library, change the version there and run the workflow.
To pin fTetWild, add `ref: <commit or tag>` to its checkout step.

Caches (`actions/cache`): vcpkg binaries (Windows), SOFA and GMSH downloads, and Qt. The
first run is slow, later runs reuse them. A cache is renewed when its key changes, e.g.
when a version number changes. Old caches can be deleted in Actions, **Caches**.

## Build and packaging

### Windows

1. Configures with the Visual Studio 2022 generator and builds `MinSizeRel`
   (`BUILD_CONFIG`).
2. `collect_portable.ps1` creates the portable folder (reads the dependency paths from
   `CMakeCache.txt`). The smoke test is skipped because the runner has no desktop.
3. `build_installer.ps1 -SkipCollect` compiles the Inno Setup installer.
4. The portable folder is zipped as `QTetraMesher_Win64_portable.zip`;
   `SHA256SUMS-windows.txt` holds the checksums of the zip and the installer.

### Linux

1. Builds `Release` with fTetWild.
2. `zz_linux_packaging/build_appimage.sh` creates the AppImage with `linuxdeploy`;
   `SOFA_LIBS_PATH` is set to the SOFA `lib` folder.
3. `SHA256SUMS-linux.txt` holds the checksum of the AppImage.

Ubuntu 22.04 is used on purpose: an AppImage only runs on systems with the same or a newer
glibc than the build machine.

## Making a release

1. Update the version in `project(QTetraMesher VERSION x.y.z)` in `CMakeLists.txt` and
   commit.
2. Optionally test with a manual run of both workflows and check the artifacts.
3. Tag and push:
   ```bash
   git tag v0.8.6
   git push origin v0.8.6
   ```
4. Both workflows run. When they are finished, the release `v0.8.6` contains the Windows
   zip and installer, the AppImage and the two checksum files. Edit the release notes on
   GitHub.

Notes:

- The tag must contain the workflow files, so the tag `0.8.5` (created before the
  workflows existed) cannot be rebuilt. Use a new tag.
- Both workflows create the release if needed. If the two jobs finish at the same time and
  one fails with a "release already exists" error, re-run the failed job.
- Remove a test release including its tag:
  ```bash
  gh release delete v0.8.6-test --cleanup-tag -y
  ```

## Troubleshooting

Read the log of the failed step in the run.

- **SOFA download fails (404):** check the asset file name on the SOFA release page and
  adjust the URL in the download step.
- **`Sofa.Config` not found:** the extracted SOFA folder layout differs from what the
  Configure step expects. The step uses the first folder inside `deps/sofa` as
  `SOFA_INSTALL_DIR`; it must contain `lib/cmake/Sofa.Config`.
- **"fTetWild module enabled" missing in the configure log:** fTetWild is skipped with a
  warning when a requirement is missing, mostly GMP. Pass `-DGMP_INCLUDE_DIRS` and
  `-DGMP_LIBRARIES` to CMake.
- **"Unresolved DLLs" warning in the Windows packaging step:** a dependency DLL was not
  found by `collect_portable.ps1`.
- **linuxdeploy aborts on a missing library:** install the package that provides it in the
  "System packages" step.
- **A stale dependency after a change:** delete the cache in Actions, **Caches**, and
  re-run.

## Windows security warnings

The binaries are not code-signed, so SmartScreen may warn on download. The checksum files
let users verify the download. Users can right-click the zip or installer, choose
Properties, and tick **Unblock** before extracting or running it.
