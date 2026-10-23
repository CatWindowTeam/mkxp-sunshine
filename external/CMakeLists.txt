# For libs we use less aggrive options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	check_option(OPTIONS_DEBUG -g -Og)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	check_option(OPTIONS_RELEASE
		-O3
		-fno-ident
		-ffunction-sections
		-fdata-sections
		-fno-rtti
		-fno-unwind-tables
		-fno-asynchronous-unwind-tables
		-fdelete-dead-exceptions
		-flto=full
		-fwhole-program-vtables
		-fvirtual-function-elimination
		-ffast-math
		-ffp-contract=fast)

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
			-mno-lvi-hardening)
		add_compile_options(${OPTIONS_RELEASE_UNSAFE})
	endif()
endif()

if(NATIVE)
	check_option(OPTIONS_NATIVE -march=native -mtune=native)
	add_compile_options(${OPTIONS_NATIVE})
endif()

add_compile_options(
    "$<$<CONFIG:Debug>:${OPTIONS_DEBUG}>"
    "$<$<CONFIG:Release>:${OPTIONS_RELEASE}>"
)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/CLI11 EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/robin-map EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/ZLIB EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/PhysFS EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_mixer EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_net EXCLUDE_FROM_ALL)
