# Platform specific bullshit
if(NOT VITA AND NOT PSP AND NOT PS2 AND NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
	set_target_properties(${APP_TARGET} PROPERTIES
    	BUILD_RPATH "$ORIGIN"
    	INSTALL_RPATH "$ORIGIN"
	)
endif()

set(UNIX_SYSTEMS
    Linux
    FreeBSD
    OpenBSD
    NetBSD
)

if(VITA)
	set(PLATFORM_LIBRARIES webp sharpyuv)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_options(${APP_TARGET} PRIVATE -municode)
    set(PLATFORM_LIBRARIES Secur32 Shlwapi winmm z mingw32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND cmake -P "${CMAKE_SOURCE_DIR}/patches/mac/CompleteBundle.cmake"
	COMMENT "Executing /patches/mac/CompleteBundle.cmake" VERBATIM)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(PLATFORM_LIBRARIES log)
    if(TERMUX)
    	find_package(PkgConfig REQUIRED)
    	pkg_check_modules(TERMUXPKGS REQUIRED libxfconf-0)
    	include_directories(${TERMUXPKGS_INCLUDE_DIRS})
    	add_compile_options(${TERMUXPKGS_CFLAGS_OTHER})
    	list(APPEND PLATFORM_LIBRARIES ${TERMUXPKGS_LDFLAGS})
    endif()
elseif(CMAKE_SYSTEM_NAME IN_LIST UNIX_SYSTEMS)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PLATFORMPKGS REQUIRED libxfconf-0)
    include_directories(${PLATFORMPKGS_INCLUDE_DIRS})
    add_compile_options(${PLATFORMPKGS_CFLAGS_OTHER})
    list(APPEND PLATFORM_LIBRARIES ${PLATFORMPKGS_LDFLAGS})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()
