# mkxp-sunshine

This is a specialized fork of [mkxp-oneshot](https://github.com/elizagamedev/mkxp-oneshot) designed for OneShot: Sunshine mod.
Target of sunshine mod - improve original game.

# xScripts.rxdata
./rpgscript.rb scripts/ [GameDir]

# Build
1.Install required packages
* Cmake
* C/C++ compiler
* xxd
* Ruby 3+
* Boost
* SDL3
* pixman
* SDL3_image
* SDL3_ttf 
* [SDL3_sound](https://github.com/icculus/SDL_sound)
* OpenAL 
* PhysFS
* sigc++-2.0
* OpenSSL
* libzip
* GTK3(Linux only!)
* libxfconf(Linux only!)

2.build

* In project dir create build dir
* cmake -S . -B build
* cd build
* make -jn (n - count of threads for compilation)
