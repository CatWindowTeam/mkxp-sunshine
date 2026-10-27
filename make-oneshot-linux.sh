#!/bin/bash
set -euo pipefail
rm -rf build journal/build libs

cmake . -B build/ "$@"
cd build
	make -j$(nproc)
cd ..

cd journal/
	bash download_journal_deps.sh
	cmake . -B build/
	cd build
		make -j$(nproc)
	cd ..
cd ..

mkdir -p build/bandle
mkdir -p build/bandle
mkdir -p build/bandle/Data
cp -r ../SunshineAssets/* build/bandle

# Copy libraries.
mkdir -p libs
ldd build/oneshot | ruby libraries.rb
cp libs/* build/bandle

# Copy binaries
cp build/oneshot build/bandle/
cp journal/build/_______ build/bandle

# Scripts files
ruby rpgscript.rb scripts/ build/bandle/

zip -X -9 -x ".git/*" "*/.git/*" -r build/OneshotSunshine_Linux.zip build/bandle/*
rm -rf libs journal/build
