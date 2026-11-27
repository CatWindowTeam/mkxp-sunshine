#!/bin/sh
# https://emscripten.org/docs/getting_started/downloads.html
cd /usr/local
git clone https://github.com/emscripten-core/emsdk.git --depth 1 --single-branch --no-tags
cd emsdk
./emsdk install latest
./emsdk activate latest
