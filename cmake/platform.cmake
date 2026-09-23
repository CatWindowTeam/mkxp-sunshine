# Platform specific bullshit
if(NOT VITA AND NOT PSP AND NOT PS2 AND NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
	set_target_properties(${APP_TARGET} PROPERTIES
    	BUILD_RPATH "$ORIGIN"
    	INSTALL_RPATH "$ORIGIN"
	)
endif()

function(detect_de_pkgs)
	find_package(PkgConfig REQUIRED)
    if(API_ONESHOT_EXTENSIONS_XFCE)
    	pkg_check_modules(XFCE IMPORTED_TARGET libxfconf-0)
    	if(XFCE_FOUND)
    	    message(STATUS "libxfconf-0 FOUND!")
    	    target_compile_definitions(${APP_TARGET} PRIVATE API_ONESHOT_EXTENSIONS_XFCE=1)
    	    list(APPEND PLATFORM_LIBRARIES PkgConfig::XFCE)
    	else()
    	    message(WARNING "libxfconf-0 NOT FOUND!")
    	    target_compile_definitions(${APP_TARGET} PRIVATE API_ONESHOT_EXTENSIONS_XFCE=0)
    	    set(API_ONESHOT_EXTENSIONS_XFCE OFF)
    	endif()
    endif()
    if(API_ONESHOT_EXTENSIONS_KDE)
    	pkg_check_modules(KDE IMPORTED_TARGET KF6ConfigCore)
    	if(KDE_FOUND)
    	    message(STATUS "KDE Frameworks config FOUND!")
    	    target_compile_definitions(${APP_TARGET} PRIVATE API_ONESHOT_EXTENSIONS_KDE=1)
    	    list(APPEND PLATFORM_LIBRARIES PkgConfig::KDE)
    	else()
    	    message(WARNING "KDE Frameworks config NOT FOUND!")
    	    target_compile_definitions(${APP_TARGET} PRIVATE API_ONESHOT_EXTENSIONS_KDE=0)
    	    set(API_ONESHOT_EXTENSIONS_KDE OFF)
    	endif()
    endif()
endfunction()

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
    add_custom_command(TARGET ${APP_TARGET} POST_BUILD COMMAND cmake -P "${CMAKE_SOURCE_DIR}/patches/mac/CompleteBundle.cmake"
	COMMENT "Executing /patches/mac/CompleteBundle.cmake" VERBATIM)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(PLATFORM_LIBRARIES log)
    if(TERMUX)
		detect_de_pkgs()
    endif()
elseif(CMAKE_SYSTEM_NAME IN_LIST UNIX_SYSTEMS)
    detect_de_pkgs()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()
