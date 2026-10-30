include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/check.cmake)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	check_option(COMPILER_OPTIONS -g -Og)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
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
		-fvirtual-function-elimination
		-fmerge-all-constants)

	check_linker_option(OPTIONS_LINKER_RELEASE
		LINKER:-O3
		LINKER:--gc-sections
		LINKER:--no-copy-dt-needed-entries
		LINKER:--icf=safe
		LINKER:--strip-all
		LINKER:--relax
		LINKER:--build-id=none
		LINKER:--as-needed
		LINKER:--no-whole-archive
		LINKER:-z,start-stop-gc
		LINKER:--no-eh-frame-hdr)
		
	if(UNSAFE_OPTIMIZATIONS)
		check_option(OPTIONS_RELEASE_UNSAFE
			-fstrub=disable
		    -fvtable-verify=none
		    -fno-enforce-eh-specs
		    -fno-hardcfr-check-exceptions
	        -fno-hardcfr-check-returning-calls
		    -fhardcfr-check-noreturn-calls=never
			-fstack-check=no
			-fno-threadsafe-statics)
		target_compile_options(${APP_TARGET} PRIVATE )
	endif()
endif()

add_link_options("$<$<CONFIG:Release>:${OPTIONS_LINKER_RELEASE}>")
target_compile_options(${APP_TARGET} PRIVATE
    -Wno-tautological-pointer-compare
    "${COMPILER_OPTIONS}"
    "$<$<CONFIG:Release>:${OPTIONS_RELEASE_UNSAFE}>"
)
