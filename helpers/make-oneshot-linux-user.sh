#this bashscript goal is to make the linux user able to compile the project easily and enjoy the game 
cmake . -B build/ "$@" -DAPI_ONESHOT_EXTENSIONS_XFCE=OFF -DAPI_ONESHOT_EXTENSIONS_KDE=OFF
cd build
make -j$(nproc)
./rpgscript.rb scripts/ $HOME/.steam/steam/steamapps/common/OneShot
cp build/oneshot $HOME/.steam/steam/steamapps/common/OneShot
