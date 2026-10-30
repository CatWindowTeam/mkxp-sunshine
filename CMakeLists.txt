cmake_minimum_required(VERSION 3.20)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are disabled. Use: cmake -S . -B build")
endif()

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release Release_LowMemory)

# LSP support
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(oneshot)

include(CheckIPOSupported)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/xxd.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/check.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/options.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ports.cmake)

# cmake/ports.cmake
ports_prepare()

if(LTO)
	check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error LANGUAGES CXX)
	if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
		if(ipo_supported)
	    	set_property(TARGET ${APP_TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
		else()
	    	message(ERROR_FATAL "IPO/LTO is not supported: ${ipo_error}")
		endif()
	else()
		message(ERROR_FATAL "IPO/LTO is not supported on Windows")
	endif()
endif()

# find src, embeds and binding src
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/source.cmake)
# add_executable
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_exec.cmake)
# platform specific deps and tweaks
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/platform.cmake)
# tweak compiler and linker for optimization
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/compiler_tweaks.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps.cmake)
add_subdirectory(scripts)
ports_finish()
message(STATUS "")
message(STATUS "----------------------------")
message(STATUS "Version ${VERSION_STRING}")
message(STATUS "Native build? ${NATIVE}")
message(STATUS "LTO: ${LTO}")
message(STATUS "[gl-mkxp] OpenGL ES 2.0 enebled? ${USE_OPENGL_ES2}")
message(STATUS "Binding: ${BINDING}")
if(VITA)
	message(STATUS "Vita build? ON")
	message(STATUS "VitaSDK path: ${VITASDK_PATH}")
elseif(PSP)
	message(STATUS "PSP build ON")
elseif(PS2)
	message(STATUS "PS2 build ON")
elseif(TERMUX)
	message(STATUS "Termux build ON")
endif()
message(STATUS "Code analysis during build enabled? ${CODE_ANAL}")
message(STATUS "Sunshine assets path ${SUNSHINE_ASSETS_PATH}")
message(STATUS "Oneshot API extensions enabled? ${API_ONESHOT_EXTENSIONS}")
if(API_ONESHOT_EXTENSIONS)
	message(STATUS "Oneshot API extensions XFCE support enabled? ${API_ONESHOT_EXTENSIONS_XFCE_ENABLED}")
endif()
message(STATUS "----------------------------")
