#!/bin/bash
# Builds QTetraMesher-x86_64.AppImage from an existing Release build in build/.
# Usage: SOFA_LIBS_PATH=/opt/sofa_git/build/install/lib zz_linux_packaging/build_appimage.sh [version]
# The version (argument, or APP_VERSION env var) defaults to the one in CMakeLists.txt
# and becomes part of the file name: QTetraMesher-<version>-x86_64.AppImage
[ -n "${BASH_VERSION:-}" ] || exec bash "$0" "$@"
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SOFA_LIBS_PATH="${SOFA_LIBS_PATH:-/opt/sofa_git/build/install/lib}"
TOOLS="$ROOT/build/appimage-tools"
APPDIR="$ROOT/build/AppDir"

mkdir -p "$TOOLS"
cd "$TOOLS"
for f in linuxdeploy-x86_64.AppImage linuxdeploy-plugin-qt-x86_64.AppImage; do
  if [ ! -x "$f" ]; then
    case "$f" in
      linuxdeploy-x86_64.AppImage)
        url=https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/$f ;;
      *)
        url=https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/$f ;;
    esac
    wget -q "$url" -O "$f"
    chmod +x "$f"
  fi
done

rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin" "$APPDIR/usr/share/qtetramesher"
cp "$ROOT/build/QTetraMesher" "$APPDIR/usr/bin/"
cp -r "$ROOT/data" "$APPDIR/usr/share/qtetramesher/"

# Lets linuxdeploy resolve the SOFA libraries (and their dependencies) and find Qt 6.
export LD_LIBRARY_PATH="$SOFA_LIBS_PATH:$ROOT/build:${LD_LIBRARY_PATH:-}"
export QMAKE="${QMAKE:-$(command -v qmake6 || command -v qmake)}"
# DEPLOY_PLATFORM_THEMES is deliberately unset: the bundled KDE theme's file dialog needs the host's KIO and showed empty folders.
# The strip bundled with linuxdeploy cannot read the .relr.dyn sections of recent distro libraries.
export NO_STRIP=1
export EXTRA_QT_PLUGINS="svg;iconengines"
DEFAULT_VERSION="$(grep -m1 -oP 'project\(QTetraMesher VERSION \K[0-9.]+' "$ROOT/CMakeLists.txt")"
export LINUXDEPLOY_OUTPUT_VERSION="${1:-${APP_VERSION:-$DEFAULT_VERSION}}"

# Bundle the project's own shared libraries too (ldd already lists them, but be explicit).
EXTRA_LIBS=()
while IFS= read -r lib; do EXTRA_LIBS+=(--library "$lib"); done \
  < <(find "$ROOT/build" -maxdepth 3 -name '*.so*' -not -path '*/_deps/*' -not -path '*/AppDir/*' -not -path '*/appimage-tools/*')

cd "$ROOT"
"$TOOLS/linuxdeploy-x86_64.AppImage" --appimage-extract-and-run \
  --appdir "$APPDIR" \
  --executable "$APPDIR/usr/bin/QTetraMesher" \
  --desktop-file "$ROOT/zz_linux_packaging/qtetramesher.desktop" \
  --icon-file "$ROOT/resources/qtetramesher_256.png" \
  --icon-filename qtetramesher \
  "${EXTRA_LIBS[@]}" \
  --plugin qt \
  --output appimage

echo "Done: $(ls "$ROOT"/QTetraMesher-"$LINUXDEPLOY_OUTPUT_VERSION"-*.AppImage)"
