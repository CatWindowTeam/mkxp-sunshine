cmake_minimum_required(VERSION 3.20)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are disabled. Use: cmake -S . -B build")
endif()
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug
    Release
)
project(oneshot)
include(CheckIPOSupported)
include(${CMAKE_SOURCE_DIR}/cmake/xxd.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/check.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/options.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/vita.cmake)

# cmake/vita.cmake
vita_prepare()

check_ipo_supported(
	RESULT ipo_supported
	OUTPUT ipo_error
	LANGUAGES CXX
)

# find src, embeds and binding src
include(${CMAKE_SOURCE_DIR}/cmake/source.cmake)
#add_executable
include(${CMAKE_SOURCE_DIR}/cmake/add_exec.cmake)
# platform specific tweaks
include(${CMAKE_SOURCE_DIR}/cmake/platform.cmake)

include(${CMAKE_SOURCE_DIR}/cmake/compiler_tweaks.cmake)

if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
	if(ipo_supported)
	    set_property(
	        TARGET ${APP_TARGET}
	        PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
	    )
	else()
	    message(WARNING "IPO/LTO is not supported: ${ipo_error}")
	endif()
endif()

include(${CMAKE_SOURCE_DIR}/cmake/deps.cmake)
add_subdirectory(scripts)
vita_finish()
