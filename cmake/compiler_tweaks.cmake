include(${CMAKE_SOURCE_DIR}/cmake/check.cmake)
include(cmake/arch_compiler_tweaks.cmake)
check_option(OPTIONS_DEBUG
	-g
	-Og)

check_option(OPTIONS_RELEASE 
	-O3
	-fno-stack-clash-protection
	-fvtable-verify=none
	-fno-ident
	-fstrub=disable
	-fno-harden-compares
	-fno-harden-conditional-branches
	-fno-harden-control-flow-redundancy
	-fno-hardcfr-check-exceptions
	-fno-hardcfr-check-returning-calls
	-fhardcfr-check-noreturn-calls=never
	-fhardcfr-skip-leaf
	-ffunction-sections
	-fdata-sections
	-fzero-call-used-regs=skip
	-fomit-frame-pointer
	-fno-rtti
	-ffp-contract=fast
	-mno-ibt)

check_option(OPTIONS_RELEASE_WINDOWS_INCOMPATIBLE
	-fno-unwind-tables
	-fno-asynchronous-unwind-tables)

check_linker_option(OPTIONS_LINKER_RELEASE
	-Wl,-O3
	-Wl,--gc-sections
	-Wl,--no-copy-dt-needed-entries
	-Wl,--icf=safe
	-Wl,--strip-all
	-Wl,--relax
	-Wl,-z,pack-relative-relocs
	-Wl,-z,noexecstack
	-Wl,--build-id=none
	-Wl,--as-needed)
add_link_options("$<$<CONFIG:Release>:${OPTIONS_LINKER_RELEASE}>")

target_compile_options(${APP_TARGET} PRIVATE
    -Wno-unused-parameter
    -Wno-tautological-pointer-compare
    "$<$<CONFIG:Debug>:${OPTIONS_DEBUG}>"
    "$<$<CONFIG:Debug>:${ARCH_DEBUG_OPTIONS}>"
    "$<$<CONFIG:Release>:${OPTIONS_RELEASE}>"
    "$<$<CONFIG:Release>:${ARCH_RELEASE_OPTIONS}>"
)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_compile_options(${APP_TARGET} PRIVATE "$<$<CONFIG:Release>:${OPTIONS_RELEASE_WINDOWS_INCOMPATIBLE}>")
endif()
