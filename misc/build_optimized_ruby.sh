#!/bin/bash
set -euo pipefail
supported_cflags=()
supported_ldflags=()
cflags=(
    -O3
    -fno-stack-protector
    -fno-stack-clash-protection
    -fno-ident
    -fstrub=disable
	-fno-harden-compares
	-fno-harden-conditional-branches
	-fno-harden-control-flow-redundancy
	-fno-hardcfr-check-exceptions
	-fno-hardcfr-check-returning-calls
	-fhardcfr-check-noreturn-calls=never
	-fhardcfr-skip-leaf
	-fzero-call-used-regs=skip
	-fomit-frame-pointer
	-fno-fast-math
	-mno-speculative-load-hardening
)

ldflags=(
	-Wl,-O3
	-Wl,--build-id=none
	-fno-stack-protector
)

# костыли:3
check_flag() {
    if printf 'int main(){return 0;}' | "${CC:-cc}" -x c -o /dev/null -Werror "$1" - >/dev/null 2>&1; then
    	echo "$1 supported"
        return 0
    else
    	echo "$1 unsupported"
        return 1
    fi
}

#checking flags
for f in "${cflags[@]}"; do
    if check_flag "$f"; then
        supported_cflags+=("$f")
    fi
done
for f in "${ldflags[@]}"; do
    if check_flag "$f"; then
        supported_ldflags+=("$f")
    fi
done

RUBY_CFLAGS="${supported_cflags[*]}" \
RUBY_LDFLAGS="${supported_ldflags[*]}" \
RUBY_CONFIGURE_OPTS="--disable-option-checking \
--enable-shared \
--enable-static \
--disable-install-doc \
--with-gmp \
--enable-yjit \
--disable-zjit \
--disable-fortify-source \
--disable-dtrace \
--disable-debug-env \
--disable-mkmf-verbose \
--disable-rubygems \
--enable-year2038 \
--without-valgrind \
--disable-pgo \
--disable-rjit \
--with-out-ext='*' \
--with-ext=zlib,monitor \
debugflags=-Wno-unused-value warnflags=-Wno-unused-value hardenflags=-Wno-unused-value" \
rbenv install 3.4.1
