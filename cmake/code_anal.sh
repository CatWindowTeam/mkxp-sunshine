#!/bin/bash
set -euo pipefail
compiler="$1"
shift
iwyu --keep define.h -Wno-unknown-arguments -Wno-unknown-warning-option -Wno-some-gcc-warning -Qunused-arguments -Xclang "$@" || echo "Install Include what you use(iwyu)"
exec "$compiler" "$@"
