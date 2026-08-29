#!/bin/bash
set -euo pipefail
# User-configurable variables.
oneshot_id="$(cat steam_appid.txt)"
STEAMWORKS_PATH=$(realpath ..)/steamworks

cmake . -B build/ "$@"
cd build
make -j$(nproc)
cd ..

mkdir -p build/bandle
mkdir -p build/bandle
mkdir -p build/bandle/Data

# Compile steamshim.
#cd steamshim_parent
#mkdir build
#cd build
#cmake -DSTEAMWORKS_PATH=${STEAMWORKS_PATH} .. > steamshim.cmake.out
#cp "$STEAMWORKS_PATH/redistributable_bin/linux64/libsteam_api.so" .
#make -j${make_threads} > steamshim.make.out
#cd ../..

ruby rpgscript.rb scripts/ build/bandle/

cp build/oneshot build/bandle/

#yes | cp steamshim_parent/build/steamshim "$ONESHOT_PATH"
#echo "$oneshot_id" > "$ONESHOT_PATH/steam_appid.txt"

# Copy libraries.
mkdir -p libs
ldd build/oneshot | ruby libraries.rb
#ldd steamshim_parent/build/steamshim | ruby libraries.rb
cp libs/* build/bandle
cp build/oneshot build/bandle

cp -r ../SunshineAssets/* build/bandle
cp oneshot.conf build/bandle
zip -9 -r build/OneshotSunshine_Linux.zip build/bandle/*

# Cleanup.
rm -rf build
rm -rf steamshim_parent/build
rm -rf journal/SDL/build
rm -rf libs
