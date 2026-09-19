#!/bin/bash
BUILD_DIR=build

set -euo pipefail # MSYS2 specific thing
ROOTDIR=$(cd $(dirname $0) && pwd)

BUILD_ROOT_DIR=$ROOTDIR/$BUILD_DIR
BUNDLING_DIR=$BUILD_ROOT_DIR/bundle # fix this typo, please
SCRIPTS_ROOT_DIR=$ROOTDIR/scripts

BUNDLE_OUT=$BUILD_ROOT_DIR/OneshotSunshine_Windows.zip

main() {
  # just in case if previous build failed
  rm -rf "$BUNDLING_DIR"

  # building
  cmake -S . -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Release
  cmake --build $BUILD_DIR

  mkdir -p "$BUNDLING_DIR"

  make_root_dir

  # making zip and removing temporary root directory
  make_zip
  rm -rf "$BUNDLING_DIR"
}

make_root_dir() {
  mkdir -p "$BUNDLING_DIR/root"

  cp -r ../SunshineAssets/* "$BUNDLING_DIR/root/"

  ruby ./rpgscript.rb "$SCRIPTS_ROOT_DIR" "$BUNDLING_DIR/root"
  
  copy_libraries
  cp "$BUILD_ROOT_DIR/oneshot" "$BUNDLING_DIR/root/"
  cp ./sunshine.conf "$BUNDLING_DIR/root/"
}

make_zip() {
  cd "$BUNDLING_DIR/root"
  zip -X -9 -x ".git/*" "*/.git/*" -r "$BUNDLE_OUT" .
}

copy_libraries() {
  mkdir -p "$BUNDLING_DIR/root"
  ldd "$BUILD_ROOT_DIR/oneshot" | ruby ./libraries.rb "$(realpath "$BUNDLING_DIR/root")/"
}

cleanup() {
  rm -rf "$BUNDLING_DIR"
}

steam() {
  echo not implemented yet.
  exit -1

  oneshot_id=$(<steam_appid.txt)
  STEAMWORKS_PATH=$(realpath ..)/steamworks

  # Compile steamshim.
  #echo -e "-> ${cyan}Compile steamshim...${color_reset}"
  #cd steamshim_parent
  #mkdir build
  #cd build
  #cmake -DSTEAMWORKS_PATH=${STEAMWORKS_PATH} .. > steamshim.cmake.out
  #cp "$STEAMWORKS_PATH/redistributable_bin/linux64/libsteam_api.so" .
  #make -j${make_threads} > steamshim.make.out
  #cd ../..

  #yes | cp steamshim_parent/build/steamshim "$ONESHOT_PATH"
  #echo "$oneshot_id" > "$ONESHOT_PATH/steam_appid.txt"
}

check_for_deps() {
  FAILED=0

  zip --version >/dev/null 2>&1 || { echo >&2 "zip is not installed."; FAILED=1; }
  cmake --version >/dev/null 2>&1 || { echo >&2 "cmake is not installed."; FAILED=1; }

  if [[ $FAILED == 1 ]] then
    exit 1
  fi
}

check_for_deps
cd $ROOTDIR && main