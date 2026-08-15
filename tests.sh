#!/bin/bash
# Scripts for static analysis
set -eo pipefail
cppcheck . --force --check-level=exhaustive -q --enable=performance

if [ -n "$1" ]; then
 find . | grep .rb | "$1"
else
  echo "path to Ruby FASTERER not present, skip"
fi
