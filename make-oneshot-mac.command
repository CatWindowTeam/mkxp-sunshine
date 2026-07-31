#!/bin/sh
set -ueo pipefail
cd `dirname $0`

# User-configurable variables
mac_version="1.1.1"
ONESHOT_PATH=$HOME/Library/Application\ Support/Steam/steamapps/common/OneShot
use_qmake=True

echo "Compiling Sunshine engine for macOS...\n"

if [[ $use_qmake == True ]]
	then
	echo "-> Generate makefile..."
	qmake MRIVERSION=3.3
	echo "-> Compile engine..."
	make -j"${nproc}"
	echo "-> Compile steamshim..."
	# cd steamshim_parent
	# mkdir build && cd build
	# cmake ..
	# STEAMWORKS=./steamworks make -j${make_threads}
	# cd ../..
else
	echo "WARNING: Conan/CMake method not ready yet."
fi
echo "-> Compile journal..."
pyinstaller journal/unix/journal.spec --onefile --windowed

# Create app bundles
echo "-> Create app bundles..."
OSX_App="OneShot.app"
ContentsDir="$OSX_App/Contents"
LibrariesDir="$OSX_App/Contents/Libraries"
ResourcesDir="$OSX_App/Contents/Resources"

# create directories in the @target@.app bundle
if [ ! -e $LibrariesDir ]
	then
	mkdir -p "$LibrariesDir"
fi

if [ ! -e $ResourcesDir ]
	then
	mkdir -p "$ResourcesDir"
fi

cp steamshim_parent/steamshim ./OneShot.app/Contents/Resources/steamshim
# cp patches/mac/libsteam_api.dylib ./OneShot.app/Contents/Libraries/libsteam_api.dylib
cp -f journal/unix/macOS/Python dist/_______.app/Contents/MacOS/Python
install_name_tool -change @loader_path/libsteam_api.dylib "$( cd "$(dirname "$0")" ; pwd -P )"/steamworks/redistributable_bin/osx32/libsteam_api.dylib ./OneShot.app/Contents/Resources/steamshim
cmake -P patches/mac/CompleteBundle.cmake
cp assets/icon.icns ./OneShot.app/Contents/Resources/icon.icns
cp steam_appid.txt ./OneShot.app/Contents/Resources/steam_appid.txt
cp patches/mac/oneshot.sh ./OneShot.app/Contents/MacOS/oneshot.sh
mv OneShot.app/Contents/MacOS/OneShot OneShot.app/Contents/Resources/OneShot
cp -r dist/_______.app _______.app

# Set version number
echo "-> Set version number..."
rm -f OneShot.app/Contents/Info.plist
rm -f _______.app/Contents/Info.plist
m4 patches/mac/Info.plist.in -DONESHOTMACVERSION=$mac_version > OneShot.app/Contents/Info.plist
m4 patches/mac/JournalInfo.plist.in -DONESHOTMACVERSION=$mac_version > _______.app/Contents/Info.plist

# Compile scripts
echo "-> Compile xScripts.rxdata..."
ruby rpgscript.rb ./scripts "$ONESHOT_PATH"
cp "$ONESHOT_PATH/Data/xScripts.rxdata" .

echo "-> Install OneShot apps to Steam directory..."
cp -rf "./OneShot.app" "$ONESHOT_PATH"
cp -rf "./_______.app" "$ONESHOT_PATH"

# Cleanup
echo "-> Cleanup files..."
# make clean
rm -rf journal/unix/__pycache__
rm -rf build
rm -rf dist
