#!/bin/bash
set -euo pipefail
PATH="$1"
PACKAGE="$2"

wget "https://mirror.msys2.org/mingw/ucrt64/mingw-w64-ucrt-x86_64-$PACKAGE.pkg.tar.zst" -O /tmp/pkg.tar.zst
tar -xf /tmp/pkg.tar.zst /tmp/pkg/
mv /tmp/pkg/ucrt64/* "$PATH"/
