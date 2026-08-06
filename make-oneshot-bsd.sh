#!/bin/sh
set -euo pipefail
cmake . -B build/
cd build
make -j$(nproc)
cd ..

mkdir -p build/bandle
mkdir -p build/bandle
mkdir -p build/bandle/Data
pyinstaller journal/unix/journal.spec #--windowed
ruby rpgscript.rb scripts/ build/bandle/

cp -r dist/_______/* build/bandle/
cp build/oneshot build/bandle/

# Copy libraries.
mkdir -p libs
ldd build/oneshot | ruby libraries.rb
#ldd steamshim_parent/build/steamshim | ruby libraries.rb
cp libs/* build/bandle
cp build/oneshot build/bandle

cp -r ../SunshineAssets/* build/bandle
cp oneshot.conf build/bandle

cd build
zip -r OneshotSunshine_BSD.zip bandle/*
cd ..

# Cleanup.
rm -rf journal/unix/__pycache__
#rm -rf build/*
rm -rf dist
#rm -rf steamshim_parent/build
rm -rf libs
