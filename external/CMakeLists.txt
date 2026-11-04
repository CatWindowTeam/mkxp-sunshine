if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	check_option(OPTIONS_DEPS -g -Og)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	check_option(OPTIONS_DEPS
		-O3
		-fno-ident
		-ffunction-sections
		-fdata-sections
		-fdelete-dead-exceptions
		-fvirtual-function-elimination
		-ffast-math
		-ffp-contract=fast
		-fmerge-all-constants
	)
		
	set(_release_cpp_options -fno-rtti -fwhole-program-vtables)
	if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
	    list(APPEND _release_cpp_options -fno-unwind-tables -fno-asynchronous-unwind-tables)
	endif()
	check_option(OPTIONS_RELEASE_CPP ${_release_cpp_options})

	if(UNSAFE_OPTIMIZATIONS)
		check_option(OPTIONS_RELEASE_UNSAFE
				-fstrub=disable
			    -fno-hardcfr-check-exceptions
		        -fno-hardcfr-check-returning-calls
		        -fhardcfr-check-noreturn-calls=never
				-fstack-check=no
		)
		
		check_option(OPTIONS_RELEASE_UNSAFE_CPP
		        -fvtable-verify=none
			    -fno-enforce-eh-specs
			    -fno-threadsafe-statics
		)
	endif()

	if(LTO)
		check_option(OPTIONS_LTO -fdevirtualize-at-ltrans)
	endif()
endif()

if(NATIVE)
	check_option(OPTIONS_NATIVE -march=native -mtune=native)
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
add_subdirectory(${CMAKE_SOURCE_DIR}/external/sigc)

# -Wno-template-body for compatibility with latest GCC
# TODO: move -Wno-template-body to libsigc++
target_compile_options(sigc-3.0 PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_CPP}" "-Wno-template-body"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_DEPS}"
	"${OPTIONS_RELEASE_UNSAFE_CPP}")
if(LTO)
	set_property(TARGET sigc-3.0 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(targets 
	SDL3_net-static
	SDL3_image-static
	SDL3-static
	SDL3_ttf-static
	SDL3_mixer-static
	zlibstatic
	physfs-static
	wavpack
	ogg
	compat
	freetype
	libmpg123
	vorbisfile
	harfbuzz)

foreach(t IN LISTS targets)
    if(TARGET "${t}")
        message(STATUS "Configuring ${t}")
        target_compile_options(${t} PRIVATE "${OPTIONS_NATIVE}"
            								"${OPTIONS_RELEASE_UNSAFE}"
            								"${OPTIONS_LTO}"
            								"${OPTIONS_DEPS}"
            								"$<$<COMPILE_LANGUAGE:CXX>:${OPTIONS_RELEASE_CPP}>"
            								"$<$<COMPILE_LANGUAGE:CXX>:${OPTIONS_RELEASE_UNSAFE_CPP}>")
		if(LTO)
			set_property(TARGET ${t} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
		endif()
    else()
        message(WARNING "${t} not found, skip")
    endif()
endforeach()
