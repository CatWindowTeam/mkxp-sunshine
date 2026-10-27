#!/bin/bash
set -euo pipefail
git clone --depth 1 --single-branch https://github.com/libsdl-org/SDL external/SDL || echo "skip"
git clone --depth 1 --single-branch https://github.com/libsdl-org/SDL_image external/SDL_image || echo "skip"
git clone --depth 1 --single-branch https://github.com/libsdl-org/SDL_net external/SDL_net || echo "skip"
cd external/SDL_image
	git submodule update --init --recursive --depth 1 --single-branch || echo "skip"
	git submodule foreach --recursive git pull || echo "skip"
cd ../..
cd external/SDL
	git submodule update --init --recursive --depth 1 --single-branch || echo "skip"
	git submodule foreach --recursive git pull || echo "skip"
cd ../..
