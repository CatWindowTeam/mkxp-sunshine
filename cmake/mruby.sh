#!/bin/bash
echo "Mruby build flags: $MRUBY_BUILD_FLAGS"
echo "Is Low memory enabled? $LOW_MEMORY"
echo "Platform: $PLATFORM"
set -euo pipefail
PPATH="$1"
cd "$PPATH/external/mruby"
MRUBY_CONFIG="$PPATH/cmake/mruby.rb" rake
