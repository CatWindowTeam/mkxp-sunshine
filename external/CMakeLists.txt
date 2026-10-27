# For libs we use less aggrive options
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
		add_compile_options(${OPTIONS_RELEASE_UNSAFE})
	endif()
endfunction()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	check_option(OPTIONS_DEBUG -g -Og)
	add_compile_options(${OPTIONS_DEBUG})
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	if(LOW_MEMORY)
		check_option(OPTIONS_RELEASE
			-Oz
			-fno-ident
			-ffunction-sections
			-fdata-sections
			-fno-rtti
			-fno-unwind-tables
			-fno-asynchronous-unwind-tables
			-fdelete-dead-exceptions
			-flto
			-fwhole-program-vtables
			-fvirtual-function-elimination
			-ffast-math
			-ffp-contract=fast
			-fshort-enums
			-fshort-wchar
			-fshort-double
			-fpack-struct=1
			-fconserve-stack)
	else()
		check_option(OPTIONS_RELEASE
			-O3
			-fno-ident
			-ffunction-sections
			-fdata-sections
			-fno-rtti
			-fno-unwind-tables
			-fno-asynchronous-unwind-tables
			-fdelete-dead-exceptions
			-flto
			-fwhole-program-vtables
			-fvirtual-function-elimination
			-ffast-math
			-ffp-contract=fast)
			add_compile_options(${OPTIONS_RELEASE})
	endif()
endif()

unsafe_optimizations()

if(NATIVE)
	check_option(OPTIONS_NATIVE -march=native -mtune=native)
	add_compile_options(${OPTIONS_NATIVE})
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/external/CLI11 EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/robin-map EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/ZLIB EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/PhysFS EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_mixer EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_net EXCLUDE_FROM_ALL)
