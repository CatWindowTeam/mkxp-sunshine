# mkxp-sunshine
This is a specialized fork of [mkxp-oneshot](https://github.com/elizagamedev/mkxp-oneshot) designed for OneShot: Sunshine mod.
Target of sunshine mod - improve original game.

## xScripts.rxdata

./rpgscript.rb scripts/ [GameDir]

## Build

1. Install required packages
    * Cmake
    * C/C++ compiler, GCC 14+ or Clang(untested) or MinGW or MSVC(untested)
    * xxd
    * Ruby 3.4+
    * Boost
    * SDL3
    * pixman
    * SDL3_image
    * SDL3_ttf
    * SDL3_mixer
    * PhysFS
    * sigc++-2.0
    * GTK3(*NIX only!)
    * libxfconf(*NIX only!)
    * libseccomp(Linux only)

(*NIX - Linux,FreeBSD and other UNIX and UNIX-like systems.)

2. build
    * In project dir create build dir
    * cmake . -B build -DDEBUG=(ON/OFF) -DCMAKE_TOOLCHAIN_FILE=toolchain/arch/(x86/arm64/ia-64).cmake
    * cd build
    * make -jn (n - count of threads for compilation)

also you can use build scripts like make-oneshot-linux.sh
