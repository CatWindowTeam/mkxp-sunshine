#!/bin/bash
export AR=$VITASDK/bin/arm-vita-eabi-ar
export AS=$VITASDK/bin/arm-vita-eabi-gcc
export CC=$VITASDK/bin/arm-vita-eabi-gcc
export CXX=$VITASDK/bin/arm-vita-eabi-g++
export LD=$VITASDK/bin/arm-vita-eabi-ld
export NM=$VITASDK/bin/arm-vita-eabi-nm
export OBJCOPY=$VITASDK/bin/arm-vita-eabi-objcopy
export OBJDUMP=$VITASDK/bin/arm-vita-eabi-objdump
export RANLIB=$VITASDK/bin/arm-vita-eabi-ranlib
export STRIP=$VITASDK/bin/arm-vita-eabi-strip

export CFLAGS="-O3 -ffast-math"
export LDFLAGS="-Wl,-O3 -Wl,-z,nocopyreloc"
export CXXFLAGS="-O3 -ffast-math"

wget https://github.com/ruby/ruby/archive/refs/tags/v3_4_1.tar.gz -O ruby.tar.gz
tar -xzf ruby.tar.gz
cd ruby-3_4_1
./autogen.sh
./configure --bindir=$VITASDK/bin \
			--sbindir=$VITASDK/bin \
			--libexecdir=$VITASDK/libexec \
			--sysconfdir=$VITASDK/etc \
			--sharedstatedir=$VITASDK/ \
			--localstatedir=$VITASDK/var \
			--libdir=$VITASDK/lib \
			--includedir=$VITASDK/include \
			--oldincludedir=$VITASDK/include \
			--datarootdir=$VITASDK/ \
			--disable-install-doc \
			--disable-yjit \
			--disable-rjit \
			--without-gmp \
			--with-static-linked-ext \
			--without-valgrind \
			--disable-shared \
			--without-dtrace \
			--disable-jit \
			--with-baseruby="$(command -v ruby)" \
			--host=arm-vita-eabi \
			ac_cv_func_dup2=no

make -j$(nproc)
