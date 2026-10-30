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
			-fdelete-dead-exceptions
			-fvirtual-function-elimination
			-ffast-math
			-ffp-contract=fast
			-fshort-enums
			-fshort-wchar
			-fshort-double
			-fpack-struct=1
			-fconserve-stack
			-fmerge-all-constants
		)
	else()
		check_option(OPTIONS_RELEASE
			-O3
			-fno-ident
			-ffunction-sections
			-fdata-sections
			-fdelete-dead-exceptions
			-fvirtual-function-elimination
			-ffast-math
			-ffp-contract=fast
		)
	endif()

	check_option(OPTIONS_RELEASE_CPP
		-fno-rtti
		-fno-unwind-tables
		-fno-asynchronous-unwind-tables
		-fwhole-program-vtables
	)

	if(UNSAFE_OPTIMIZATIONS)
		check_option(OPTIONS_RELEASE_UNSAFE
				-fstrub=disable
			    -fno-hardcfr-check-exceptions
		        -fno-hardcfr-check-returning-calls
		        -fhardcfr-check-noreturn-calls=never
				-fstack-check=no
				-mno-speculative-load-hardening
				-fzero-call-used-regs=skip
				-mno-lvi-hardening
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
set(SIGCPP_STATIC ON CACHE BOOL "Build Static library?")
set(SIGCPP_EXAMPLES OFF CACHE BOOL "Build examples")
set(SIGCPP_TESTS OFF CACHE BOOL "Build tests")
add_subdirectory(${CMAKE_SOURCE_DIR}/external/sigc)
if(LTO)
	set_property(TARGET SDL3_net-static PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET zlibstatic PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET physfs-static PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET wavpack PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET ogg PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET compat PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET sigc-3.0 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET SDL3_image-static PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET freetype PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET vorbis PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET libmpg123 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET harfbuzz PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET vorbisfile PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET SDL3-static PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET SDL3_ttf-static PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET SDL3_mixer-static PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_compile_options(SDL3_net-static PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")
	
target_compile_options(zlibstatic PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")
	
target_compile_options(physfs-static PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")
	
target_compile_options(wavpack PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")
	
target_compile_options(ogg PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")
	
target_compile_options(compat PRIVATE "${OPTIONS_NATIVE}"
	"$<$<COMPILE_LANGUAGE:CXX>:${OPTIONS_RELEASE_CPP}>"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}"
	"$<$<COMPILE_LANGUAGE:CXX>:${OPTIONS_RELEASE_UNSAFE_CPP}>")

# -Wno-template-body for compatibility with latest GCC
target_compile_options(sigc-3.0 PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_CPP}" "-Wno-template-body"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}"
	"${OPTIONS_RELEASE_UNSAFE_CPP}")
	
target_compile_options(SDL3_image-static PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")
	
target_compile_options(freetype PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")

target_compile_options(vorbis PRIVATE "${OPTIONS_NATIVE}"
	"$<$<COMPILE_LANGUAGE:CXX>:${OPTIONS_RELEASE_CPP}>"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}"
	"$<$<COMPILE_LANGUAGE:CXX>:${OPTIONS_RELEASE_UNSAFE_CPP}>")

target_compile_options(libmpg123 PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")

target_compile_options(harfbuzz PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_CPP}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}"
	"${OPTIONS_RELEASE_UNSAFE_CPP}")
	
target_compile_options(vorbisfile PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")

target_compile_options(SDL3-static PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")
	
target_compile_options(SDL3_ttf-static PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")
	
target_compile_options(SDL3_mixer-static PRIVATE "${OPTIONS_NATIVE}"
	"${OPTIONS_RELEASE_UNSAFE}" "${OPTIONS_LTO}" "${OPTIONS_RELEASE}")
