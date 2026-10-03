#!/bin/bash
set -euo pipefail
cd $PS2DEV
git clone --depth 1 https://github.com/libsdl-org/SDL
cd SDL
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE=$PS2DEV/share/ps2dev.cmake \
	-DSDL_VIRTUAL_JOYSTICK=OFF \
	-DSDL_TESTS=OFF \
	-DSDL_OPENGL=ON \
	-DSDL_OPENGLES=ON \
	-DSDL_OFFSCREEN=OFF \
	-DSDL_GPU_OPENXR=OFF \
	-DSDL_DUMMYVIDEO=OFF \
	-DSDL_DUMMYCAMERA=OFF \
	-DSDL_DUMMYAUDIO=OFF \
	-DSDL_DISKAUDIO=OFF \
	-DSDL_EXAMPLES=OFF \
	-DSDL_TESTS=OFF \
	-DSDL_TRAY=OFF \
	-DSDL_CAMERA=OFF \
	-DSDL_DISABLE_INSTALL_DOCS=ON \
	-DSDL_NOTIFICATION=OFF \
	-DSDL_POWER=OFF
make -j$(nproc)
cd ..

git clone --depth 1 https://github.com/madler/zlib
cd zlib
cmake . \
	-DCMAKE_TOOLCHAIN_FILE=$PS2DEV/share/ps2dev.cmake \
	-DZLIB_BUILD_TESTING=OFF \
	-DZLIB_BUILD_SHARED=OFF \
	-DZLIB_BUILD_STATIC=ON \
	-DCMAKE_BUILD_TYPE=Release \
	-DZLIB_INSTALL=OFF
make -j$(nproc)
cd ..

git clone --depth 1 https://github.com/libsdl-org/SDL_image
cd SDL_image
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE=$PS2DEV/share/ps2dev.cmake \
	-DSDL3_DIR=$PS2DEV/SDL \
	-DZLIB_DIR=$PS2DEV/zlib
make -j$(nproc)
cd ..
