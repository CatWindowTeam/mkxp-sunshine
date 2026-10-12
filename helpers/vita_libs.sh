#!/bin/bash
set -euo pipefail
cd $VITASDK
ls $VITASDK

vdpm install pixman \
			 physfs \
			 libsigc++-2.0 \
			 zlib

git clone --depth 1 https://github.com/libsdl-org/SDL
cd SDL
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake \
	-DSDL_VIRTUAL_JOYSTICK=OFF \
	-DSDL_TESTS=OFF \
	-DSDL_OPENGL=OFF \
	-DSDL_OPENGLES=OFF \
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
	-DSDL_GPU=OFF \
	-DSDL_DISABLE_INSTALL_DOCS=ON \
	-DSDL_NOTIFICATION=OFF \
	-DSDL_POWER=OFF \
	-DSDL_UNINSTALL=OFF \
	-DSDL_SYSTEM_ICONV=OFF
make -j$(nproc)
cd ..

git clone --depth 1 https://github.com/libsdl-org/SDL_image
cd SDL_image
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake \
	-DSDL3_DIR="$VITASDK/SDL" \
	-DSDLIMAGE_TIF=OFF \
	-DBUILD_SHARED_LIBS=OFF \
	-DPLATFORM_SUPPORTS_SHARED=OFF \
	-DSDLIMAGE_DEPS_SHARED=OFF \
	-DSDLIMAGE_SAMPLES=OFF \
	-DSDLIMAGE_TESTS=OFF \
	-DSDLIMAGE_ANI=OFF \
	-DSDLIMAGE_AVIF=OFF \
	-DSDLIMAGE_BMP=OFF \
	-DSDLIMAGE_GIF=OFF \
	-DSDLIMAGE_JPG=OFF \
	-DSDLIMAGE_JXL=OFF \
	-DSDLIMAGE_LBM=OFF \
	-DSDLIMAGE_PCX=OFF \
	-DSDLIMAGE_PNG=ON \
	-DSDLIMAGE_PNM=OFF \
	-DSDLIMAGE_QOI=OFF \
	-DSDLIMAGE_SVG=OFF \
	-DSDLIMAGE_TGA=OFF \
	-DSDLIMAGE_WEBP=OFF \
	-DSDLIMAGE_XCF=OFF \
	-DSDLIMAGE_XPM=OFF \
	-DSDLIMAGE_XV=OFF
make -j$(nproc)
cd ..

git clone --depth 1 https://github.com/libsdl-org/SDL_mixer
cd SDL_mixer
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake \
	-DSDL3_DIR="$VITASDK/SDL" \
	-DSDLMIXER_GME=OFF \
	-DPLATFORM_SUPPORTS_SHARED=OFF \
	-DSDLMIXER_INSTALL=OFF \
	-DSDLMIXER_DEPS_SHARED=OFF \
	-DSDLMIXER_TESTS=OFF \
	-DSDLMIXER_EXAMPLES=OFF \
	-DSDLMIXER_AIFF=OFF \
	-DSDLMIXER_WAVE=OFF \
	-DSDLMIXER_VOC=OFF \
	-DSDLMIXER_AU=OFF \
	-DSDLMIXER_FLAC=OFF \
	-DSDLMIXER_MOD=OFF \
	-DSDLMIXER_MP3=ON \
	-DSDLMIXER_MIDI=OFF \
	-DSDLMIXER_OPUS=OFF \
	-DSDLMIXER_VORBIS_STB=OFF \
	-DSDLMIXER_VORBIS_VORBISFILE=OFF \
	-DSDLMIXER_VORBIS_TREMOR=OFF \
	-DSDLMIXER_VORBIS_VORBISFILE=OFF \
	-DSDLMIXER_WAVPACK=OFF \
	-DBUILD_SHARED_LIBS=OFF
make -j$(nproc)
cd ..

git clone --depth 1 https://github.com/libsdl-org/SDL_ttf
cd SDL_ttf
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake \
	-DSDL3_DIR="$VITASDK/SDL" \
	-DBUILD_SHARED_LIBS=OFF \
	-DSDLTTF_SAMPLES=OFF \
	-DSDLTTF_BUILD_SHARED_LIBS=OFF \
	-DPLATFORM_SUPPORTS_SHARED=OFF
make -j${nproc}
cd ..

git clone --depth 1 https://github.com/libsdl-org/SDL_net
cd SDL_net
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake \
	-DSDL3_DIR="$VITASDK/SDL" \
	-DBUILD_SHARED_LIBS=OFF \
	-DSDLNET_SAMPLES=OFF \
	-DSDLNET_BUILD_SHARED_LIBS=OFF \
	-DPLATFORM_SUPPORTS_SHARED=OFF
make -j${nproc}
cd ..

git clone --depth 1 https://github.com/AnmiTaliDev/ruby
cd ruby
./autogen.sh
CC="arm-vita-eabi-gcc" \
CXX="arm-vita-eabi-g++" \
CPP="arm-vita-eabi-cpp" \
AR="arm-vita-eabi-ar" \
AS="arm-vita-eabi-as" \
NM="arm-vita-eabi-nm" \
OBJCOPY="arm-vita-eabi-objcopy" \
OBJDUMP="arm-vita-eabi-objdump" \
RANLIB="arm-vita-eabi-ranlib" \
STRIP="arm-vita-eabi-strip" \
	./configure --disable-option-checking \
	--with-static-linked-ext \
	--host="arm-vita-eabi" \
	--target="arm-vita-eabi" \
	--build="$(./tool/config.guess)" \
	--disable-shared \
	--enable-static \
	--disable-install-doc \
	--without-gmp \
	--disable-yjit \
	--disable-zjit \
	--disable-fortify-source \
	--disable-dtrace \
	--disable-debug-env \
	--disable-mkmf-verbose \
	--disable-rubygems \
	--disable-year2038 \
	--without-valgrind \
	--disable-pgo \
	--disable-rjit \
	--with-out-ext='*' \
	--with-ext= \
	--disable-rubygems \
	--disable-dln \
	--disable-largefile \
	--with-baseruby="$(command -v ruby)" \
	--with-compress-debug-sections=no \
	debugflags=-Wno-unused-value warnflags=-Wno-unused-value hardenflags=-Wno-unused-value
make -j${nproc}
cd ..
