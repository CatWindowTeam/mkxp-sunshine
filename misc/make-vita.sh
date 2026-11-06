#!/bin/bash
set -euo pipefail
shopt -u dotglob
rm -rf build

cmake . -B build/ -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake -DRENDER_GPU=OFF -DRENDER_GL_MKXP=OFF "$@"
cmake --build build/ "-j$(nproc)"

### building bundle ###
# ONESHOT_ASSETS must contain all assets(Oneshot + Sunshine), they will be included in VPK
zip -X -9 -r build/assets.zip $ONESHOT_ASSETS/ -x ".*"

vita-elf-create -svvv build/oneshot.elf build/oneshot.velf
psp2rela -src=build/oneshot.velf -dst=build/oneshot.velf
vita-make-fself -na -c -s build/oneshot.velf build/eboot.bin
vita-mksfoex -s TITLE_ID=OSSS6691 "Oneshot: Sunshine" build/param.sfo
vita-pack-vpk -s build/param.sfo -b build/eboot.bin \
  -a sce_sys/icon0.png=misc/sce_sys/icon0.png \
  -a sce_sys/livearea/contents/bg.png=misc/sce_sys/livearea/contents/bg.png \
  -a sce_sys/livearea/contents/startup.png=misc/sce_sys/livearea/contents/startup.png \
  -a sce_sys/livearea/contents/template.xml=misc/sce_sys/livearea/contents/template.xml \
  -a sunshine.zip=build/assets.zip \
  build/sunshine.vpk
