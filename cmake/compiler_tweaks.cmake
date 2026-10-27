include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/check.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/arch_compiler_tweaks.cmake)

function(unsafe_optimizations)
	if(UNSAFE_OPTIMIZATIONS)
		check_option(OPTIONS_RELEASE_UNSAFE
			-fstrub=disable
		    -fvtable-verify=none
		    -fno-enforce-eh-specs
		    -fno-hardcfr-check-exceptions
	        -fno-hardcfr-check-returning-calls
		    -fhardcfr-check-noreturn-calls=never
			-fstack-check=no
			-mno-speculative-load-hardening
			-fzero-call-used-regs=skip
			-fno-threadsafe-statics
			-mno-lvi-hardening)
		target_compile_options(${APP_TARGET} PRIVATE ${OPTIONS_RELEASE_UNSAFE})
	endif()
endfunction()

function(release_linker_tweaks)
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
endfunction()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	check_option(OPTIONS_DEBUG -g -Og)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	if(LOW_MEMORY)
		check_option(COMPILER_OPTIONS
			-Oz
			-fno-ident
			-ffunction-sections
			-fdata-sections
			-fno-rtti
			-ffp-contract=fast
			-fno-unwind-tables
			-fno-asynchronous-unwind-tables
			-fdelete-dead-exceptions
			-fwhole-program-vtables
			-fvirtual-function-elimination
			-fshort-enums
			-fshort-wchar
			-fshort-double
			-fpack-struct=1
			-fconserve-stack)
	else()
		check_option(COMPILER_OPTIONS
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
	endif()

	release_linker_tweaks()
	unsafe_optimizations()
endif()

add_link_options("$<$<CONFIG:Release>:${OPTIONS_LINKER_RELEASE}>")
target_compile_options(${APP_TARGET} PRIVATE
    -Wno-tautological-pointer-compare
    "$<$<CONFIG:Debug>:${COMPILER_OPTIONS}>"
    "$<$<CONFIG:Debug>:${ARCH_DEBUG_OPTIONS}>"
    "$<$<CONFIG:Release>:${ARCH_RELEASE_OPTIONS}>"
)
