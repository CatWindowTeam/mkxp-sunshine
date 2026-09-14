#!/bin/bash
set -euo pipefail
cmake . -B build/ "$@"

cd build
	make -j$(nproc)
cd ..

mkdir -p build/oneshot.AppDir
mkdir -p build/oneshot.AppDir/lib/
cp oneshot.AppDir/* build/oneshot.AppDir/
cp build/oneshot build/oneshot.AppDir/
patchelf --set-rpath '$ORIGIN/lib' build/oneshot.AppDir/oneshot
# Copy libraries.
mkdir -p libs
ldd build/oneshot | ruby libraries.rb
cp libs/* build/oneshot.AppDir/lib/

ARCH=x86_64 ./appimagetool-x86_64.AppImage build/oneshot.AppDir/ build/oneshot-x86_64.AppImage

rm -rf journal/SDL/build
rm -rf libs
