#!/bin/bash
#
# ПЕРЕПИСАТЬ ЭТУ ХУЙНЮ ОНА НЕ РАБОТАЕТ
#

set -euo pipefail
FASTERER_PATH="$1"

check_c(){
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

check_shader(){
  local f="$1"
  case "$f" in
    *.vert)
		#glslangValidator -S vert --target-env opengl --client opengl100 -t "$f"
    ;;
    *.frag)
		glslangValidator -S frag --target-env opengl --client opengl100 -t -DGLSLES -DFRAGMENT_SHADER "$f"
    ;;
  esac
}
export -f check_c
export -f check_shader

find . -type f \( \
  -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' \
  -o -name '*.h' -o -name '*.hh' -o -name '*.hpp' -o -name '*.hxx' \
\) -print0 \
| xargs -0 -n 1 bash -c 'check_c "$1"' _

find . -type f \( \
  -name '*.frag' -o -name '*.vert' \
\) -print0 \
| xargs -0 -n 1 bash -c 'check_shader "$1"' _

find . | grep .rb | $FASTERER_PATH
glslangValidator
