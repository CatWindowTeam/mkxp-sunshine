#!/bin/bash
set -euo pipefail
cd $PS2DEV
ls $PS2SDK
git clone --depth 1 https://github.com/libsdl-org/SDL
cd SDL
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE=$PS2DEV/share/ps2dev.cmake \
	-DCMAKE_PREFIX_PATH="$PS2SDK/ports" \
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

git clone --depth 1 https://github.com/libsdl-org/SDL_image
echo """set(ZLIB_LIBRARY
    "$PS2SDK/ports/lib/libz.a"
    CACHE FILEPATH "PS2 zlib library" FORCE)

set(ZLIB_INCLUDE_DIR
    "$PS2SDK/ports/include"
    CACHE PATH "PS2 zlib include directory" FORCE)

set(ZLIB_INCLUDE_DIRS
    "${ZLIB_INCLUDE_DIR}"
    CACHE PATH "PS2 zlib include directories" FORCE)

if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB STATIC IMPORTED GLOBAL)
    set_target_properties(ZLIB::ZLIB PROPERTIES
        IMPORTED_LOCATION "${ZLIB_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIR}"
    )
endif()""" > /tmp/zlib-init.cmake
cd SDL_image
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE="$PS2DEV/share/ps2dev.cmake" \
	-DSDL3_DIR="$PS2DEV/SDL" \
	-DCMAKE_PREFIX_PATH="$PS2SDK/ports" \
	-DSDLIMAGE_TIF=OFF \
	-DBUILD_SHARED_LIBS=OFF \
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
	-DSDLIMAGE_XV=OFF \
	-DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=/tmp/zlib-init.cmake
make -j$(nproc)
cd ..

git clone --depth 1 https://github.com/libsdl-org/SDL_mixer
cd SDL_mixer
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE="$PS2DEV/share/ps2dev.cmake" \
	-DSDL3_DIR="$PS2DEV/SDL" \
	-DCMAKE_PREFIX_PATH="$PS2SDK/ports"
make -j$(nproc)
cd ..

git clone --depth 1 https://github.com/Tessil/robin-map
cd robin-map
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE=$PS2DEV/share/ps2dev.cmake \
	-DCMAKE_PREFIX_PATH="$PS2SDK/ports"
make -j$(nproc)
cd ..

git clone --depth 1 https://github.com/AnmiTaliDev/physfs
cd physfs
cmake . \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE=$PS2DEV/share/ps2dev.cmake \
	-DCMAKE_PREFIX_PATH="$PS2SDK/ports" \
	-DPHYSFS_ARCHIVE_7Z=OFF \
	-DPHYSFS_ARCHIVE_GRP=OFF \
	-DPHYSFS_ARCHIVE_WAD=OFF \
	-DPHYSFS_ARCHIVE_HOG=OFF \
	-DPHYSFS_ARCHIVE_MVL=OFF \
	-DPHYSFS_ARCHIVE_QPAK=OFF \
	-DPHYSFS_ARCHIVE_SLB=OFF \
	-DPHYSFS_ARCHIVE_ISO9660=OFF \
	-DPHYSFS_ARCHIVE_VDF=OFF \
	-DPHYSFS_BUILD_STATIC=ON \
	-DPHYSFS_BUILD_SHARED=OFF \
	-DPHYSFS_BUILD_TEST=OFF \
	-DPHYSFS_DISABLE_INSTALL=ON \
	-DPHYSFS_BUILD_DOCS=OFF
make -j8
cd ..
	
git clone --depth 1 https://github.com/AnmiTaliDev/ruby
cd ruby
./autogen.sh
CC="mips64r5900el-ps2-elf-gcc" \
CXX="mips64r5900el-ps2-elf-g++" \
CPP="mips64r5900el-ps2-elf-cpp" \
AR="mips64r5900el-ps2-elf-ar" \
AS="mips64r5900el-ps2-elf-as" \
NM="mips64r5900el-ps2-elf-nm" \
OBJCOPY="mips64r5900el-ps2-elf-objcopy" \
OBJDUMP="mips64r5900el-ps2-elf-objdump" \
RANLIB="mips64r5900el-ps2-elf-ranlib" \
STRIP="mips64r5900el-ps2-elf-strip" \
	./configure --disable-option-checking \
	--with-static-linked-ext \
	--host="mips64r5900el-ps2-elf" \
	--target="mips64r5900el-ps2-elf" \
	--build="$(./tool/config.guess)" \
	--prefix="$PS2SDK/ports" \
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
make -j8
cd ..
