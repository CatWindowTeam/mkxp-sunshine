cmake_minimum_required(VERSION 3.20)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are disabled. Use: cmake -S . -B build")
endif()

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)

project(oneshot)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/xxd.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/check.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/data.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/options.cmake)
include(CheckIPOSupported)

# LTO
if(LTO)
	check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error LANGUAGES CXX)
	if(NOT MinGW)
		if(ipo_supported)
	    	set_property(TARGET ${APP_TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
		else()
	    	message(FATAL_ERROR "IPO/LTO is not supported: ${ipo_error}")
		endif()
	else()
		message(FATAL_ERROR "IPO/LTO is not supported on MinGW/MSYS2")
	endif()
endif()

# find src, embeds and binding src
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/source.cmake)
# add_executable
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_exec.cmake)
# configure binding
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/binding.cmake)
# platform specific deps and tweaks
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/platform.cmake)
# tweak compiler and linker for optimization
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/compiler_tweaks.cmake)
# meow
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps.cmake)

# xScripts.rxdata
if(BUILD_XSCRIPTS)
	add_subdirectory(scripts)
endif()

message(STATUS "")
message(STATUS "----------------------------")
message(STATUS "Version ${VERSION_STRING}")
message(STATUS "Native build? ${NATIVE}")
message(STATUS "LTO: ${LTO}")
message(STATUS "OpenGL ES 2.0 in gl-mkxp enebled? ${USE_OPENGL_ES2}")
message(STATUS "Binding: ${BINDING}")
message(STATUS "Build xScripts.rxdata? ${BUILD_XSCRIPTS}")
if(VITA)
	message(STATUS "Vita build? ON")
elseif(TERMUX)
	message(STATUS "Termux build ON")
endif()
if(API_ONESHOT_EXTENSIONS)
	message(STATUS "Oneshot API extensions XFCE support enabled? ${API_ONESHOT_EXTENSIONS_XFCE_ENABLED}")
endif()
message(STATUS "----------------------------")
