#!/bin/bash
set -euo pipefail
PPATH="$1"
cd "$PPATH/external/mruby"
MRUBY_CONFIG="$PPATH/cmake/mruby.rb" rake
