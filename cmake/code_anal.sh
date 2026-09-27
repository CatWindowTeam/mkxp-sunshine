#!/bin/bash
set -euo pipefail
compiler="$1"
shift
#maybe add some custom analyze and profiling here?
exec "$compiler" "$@"
