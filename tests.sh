#!/bin/bash
set -euo pipefail
FASTERER_PATH="$1"

check_file() {
  local f="$1"

  case "$f" in
    *.c|*.cc|*.cpp|*.cxx)
      #cppcheck --check-level=exhaustive --disable=missingInclude --enable=all --force -q -I . "$f" \
      #  || echo "cppcheck is not installed or reported issues for: $f"

      #gcc -fanalyzer -Wall -Wextra -Wpedantic -std=c11 -c "$f" -o /dev/null 2>/dev/null \
      #  || echo "gcc is not installed or analyzer reported issues for: $f"

      cbmc "$f" \
        || echo "cbmc is not installed or reported issues for: $f"
      ;;
    *.h|*.hh|*.hpp|*.hxx)
      cppcheck --check-level=exhaustive --enable=all --disable=missingInclude -q "$f" \
        || echo "cppcheck is not installed or reported issues for header: $f"
      ;;
  esac
}
export -f check_file

find . -type f \( \
  -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' \
  -o -name '*.h' -o -name '*.hh' -o -name '*.hpp' -o -name '*.hxx' \
\) -print0 \
| xargs -0 -n 1 bash -c 'check_file "$1"' _

find . | grep .rb | $FASTERER_PATH
