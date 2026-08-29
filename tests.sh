#!/bin/bash
# Scripts for static code analysis
set -eo pipefail
cppcheck . --force --check-level=exhaustive -q --enable=performance

# TODO: Find gopd aliternative for fasterer
if [ -n "$1" ]; then
 find . | grep .rb | "$1"
else
  echo "path to Ruby FASTERER not present, skip"
fi
