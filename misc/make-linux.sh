#!/bin/bash
set -euo pipefail
shopt -u dotglob
rm -rf build journal/build libs

cmake . -B build/ "$@"
cmake --build build/ "-j$(nproc)"

cd journal/
	git submodule update --init --recursive --single-branch --jobs $(nproc)
	git submodule foreach --recursive git pull
	cmake . -B build/ "$@"
	cmake --build build/ "-j$(nproc)"
cd ..

mkdir -p build/bandle
mkdir -p build/bandle
mkdir -p build/bandle/Data
cp -r /assets/* build/bandle

# Copy libraries.
mkdir -p libs
ldd build/oneshot | ruby libraries.rb
cp libs/* build/bandle

# Copy binaries
cp build/oneshot build/bandle/
cp journal/build/_______ build/bandle

# Scripts files
ruby rpgscript.rb scripts/ build/bandle/

cd build/bandle/
	tar -I 'xz -9e' -cf ../OneshotSunshine_Linux.tar.xz --exclude='.git' *
cd ../..

rm -rf libs journal/build
