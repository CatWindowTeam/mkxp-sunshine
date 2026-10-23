include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/check.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/arch_compiler_tweaks.cmake)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	check_option(OPTIONS_DEBUG
		-g
		-Og)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	check_option(OPTIONS_RELEASE
		-O3
		-fno-ident
		-ffunction-sections
		-fdata-sections
		-fno-rtti
		-ffp-contract=fast
		-fno-unwind-tables
		-fno-asynchronous-unwind-tables
		-fdelete-dead-exceptions
		-fwhole-program-vtables
		-fvirtual-function-elimination)

	check_linker_option(OPTIONS_LINKER_RELEASE
		-Wl,-O3
		-Wl,--gc-sections
		-Wl,--no-copy-dt-needed-entries
		-Wl,--icf=safe
		-Wl,--strip-all
		-Wl,--relax
		-Wl,--build-id=none
		-Wl,--as-needed
		-Wl,--no-insert-timestamp
		-Wl,--no-whole-archive
		-Wl,-z,start-stop-gc
		-Wl,--no-eh-frame-hdr)

	if(UNSAFE_OPTIMIZATIONS)
		check_option(OPTIONS_RELEASE_UNSAFE
			-fstrub=disable
            -fno-stack-clash-protection
            -fno-stack-protector
            -fvtable-verify=none
            -fno-enforce-eh-specs
            -fno-harden-compares
            -fno-harden-conditional-branches
            -fno-harden-control-flow-redundancy
            -fno-hardcfr-check-exceptions
            -fno-hardcfr-check-returning-calls
            -fhardcfr-check-noreturn-calls=never
			-fno-stack-check
			-mno-speculative-load-hardening
			-fzero-call-used-regs=skip
			-fzero-init-padding-bits=standard
			-fno-threadsafe-statics
			-mno-lvi-hardening)
		target_compile_options(${APP_TARGET} PRIVATE ${OPTIONS_RELEASE_UNSAFE})
	endif()
endif()

add_link_options("$<$<CONFIG:Release>:${OPTIONS_LINKER_RELEASE}>")
target_compile_options(${APP_TARGET} PRIVATE
    -Wno-tautological-pointer-compare
    "$<$<CONFIG:Debug>:${OPTIONS_DEBUG}>"
    "$<$<CONFIG:Debug>:${ARCH_DEBUG_OPTIONS}>"
    "$<$<CONFIG:Release>:${OPTIONS_RELEASE}>"
    "$<$<CONFIG:Release>:${ARCH_RELEASE_OPTIONS}>"
)
