#!/bin/bash
set -euo pipefail

cmake . -B build/ "$@"
cd build
make -j$(nproc)
cd ..

mkdir -p build/bandle
mkdir -p build/bandle
mkdir -p build/bandle/Data

ruby rpgscript.rb scripts/ build/bandle/
cp build/oneshot build/bandle/

# Copy libraries.
mkdir -p libs
ldd build/oneshot | ruby libraries.rb
cp libs/* build/bandle
cp build/oneshot build/bandle

cp -r ../SunshineAssets/* build/bandle
cp oneshot.conf build/bandle
zip -X -9 -x ".git/*" "*/.git/*" -r build/OneshotSunshine_Linux.zip build/bandle/*

rm -rf libs
