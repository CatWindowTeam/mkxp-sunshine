#!/bin/sh
set -e

cd `dirname $0`

# User-configurable variables.
linux_version="0.1.0"
oneshot_id=420530
STEAMWORKS_PATH=$(realpath ..)/steamworks

# Colors.
white="\033[0;37m"      # White - Regular
bold="\033[1;37m"       # White - Bold
cyan="\033[1;36m"       # Cyan - Bold
green="\033[1;32m"      # Green - Bold
color_reset="\033[0m"   # Reset Colors

echo -e "${white}Compiling ${bold}SyngleChance v${linux_version} ${white}engine for Linux...${color_reset}\n"

cmake . -B build/
cd build
make -j${nproc}
cd ..

mkdir build/bandle
mkdir build/bandle/Sunshine
mkdir build/bandle/Sunshine/Data

# Compile steamshim.
#echo -e "-> ${cyan}Compile steamshim...${color_reset}"
#cd steamshim_parent
#mkdir build
#cd build
#cmake -DSTEAMWORKS_PATH=${STEAMWORKS_PATH} .. > steamshim.cmake.out
#cp "$STEAMWORKS_PATH/redistributable_bin/linux64/libsteam_api.so" .
#make -j${make_threads} > steamshim.make.out
#cd ../..

# Compile Journal.
echo -e "-> ${cyan}Compile journal...${color_reset}"
pyinstaller journal/unix/journal.spec #--windowed

# Compile scripts.
echo -e "-> ${cyan}Compile xScripts.rxdata...${color_reset}"
ruby rpgscript.rb scripts/ build/bandle/Sunshine/

# Copy results.
echo -e "-> ${cyan}Install OneShot apps to Steam directory...${color_reset}"
yes | cp -r dist/_______/* build/bandle/Sunshine/
yes | cp build/oneshot build/bandle/Sunshine

#yes | cp steamshim_parent/build/steamshim "$ONESHOT_PATH"
#echo "$oneshot_id" > "$ONESHOT_PATH/steam_appid.txt"

# Copy libraries.
echo -e "-> ${cyan}Install OneShot libraries to Steam directory...${color_reset}"
mkdir libs
ldd build/oneshot | ruby libraries.rb
#ldd steamshim_parent/build/steamshim | ruby libraries.rb
yes | cp libs/* build/bandle/Sunshine
yes | cp build/oneshot build/bandle/Sunshine

cp installer/installer.sh build/bandle/
cp -r ../SunshineAssets/* build/bandle/Sunshine
cp assets/oneshot.png build/bandle/Sunshine

cd build
zip -r OneshotSunshine.zip bandle/*
cd ..

# Cleanup.
echo -e "-> ${cyan}Cleanup files...${color_reset}"
rm -rf journal/unix/__pycache__
#rm -rf build/*
rm -rf dist
#rm -rf steamshim_parent/build
rm -rf libs
