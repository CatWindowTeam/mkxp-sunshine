# Sunshine Engine
It is a specialized fork of [mkxp-oneshot](https://github.com/elizagamedev/mkxp-oneshot) developed for the OneShot: Sunshine mod, aimed at optimization, running on old and new hardware, support for multiple platforms, and mod download security.

## Build
1. Install required packages
    * Cmake
    * C/C++ compiler, GCC 14, MinGW, Clang and maybe MSVC
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
Use build scripts.
make-oneshot-(OS).sh -DCMAKE_TOOLCHAIN_FILE=toolchain/arch/(x86/arm/ia-64).cmake
