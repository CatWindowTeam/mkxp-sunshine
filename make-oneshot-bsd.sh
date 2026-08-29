#!/bin/sh
cmake . -B build/ -DDEBUG=OFF
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
#ldd steamshim_parent/build/steamshim | ruby libraries.rb
cp libs/* build/bandle
cp build/oneshot build/bandle

cp -r ../SunshineAssets/* build/bandle
cp oneshot.conf build/bandle

cd build
zip -r OneshotSunshine_BSD.zip bandle/*
cd ..

# Cleanup.
rm -rf build
rm -rf libs
