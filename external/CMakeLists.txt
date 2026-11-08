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
	"${COPT_C}" "-Wno-template-body"
	"${COPT_UNSAFE}" "${COPT_LTO}" "${COPT_CXX}"
	"${COPT_UNSAFE_CXX}" "${COPT_NATIVE}"
)

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
	harfbuzz
	plutosvg
)

foreach(t IN LISTS targets)
    if(TARGET "${t}")
        message(STATUS "Configuring ${t}")
        target_compile_options(${t} PRIVATE "${COPT_NATIVE}"
            								"${COPT_UNSAFE}"
            								"${COPT_LTO}"
            								"${COPT_C}"
            								"$<$<COMPILE_LANGUAGE:CXX>:${COPT_CXX}>"
            								"$<$<COMPILE_LANGUAGE:CXX>:${COPT_UNSAFE_CXX}>")
		if(LTO)
			set_property(TARGET ${t} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
		endif()
    else()
        message(WARNING "${t} not found, skip")
    endif()
endforeach()
