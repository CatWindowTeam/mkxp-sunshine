# Options
option(STEAM "Build with Steam Support" OFF)
option(NATIVE "Use native instructions,for local use only" OFF)
option(STATIC "Build more static build" OFF)
option(USE_OPENGL_ES2 "GLES2_HEADER define" OFF)
set(VERSION_STRING "0.1.3" CACHE STRING "Version string")
set(BINDING_PATH "binding-mri" CACHE STRING "Binding to use")
set(VITASDK_PATH "/usr/local/vitasdk" CACHE STRING "VitaSDK path")
option(DEVBUILD "Developoment build" OFF)
option(TERMUX "Build for TERMUX" OFF)
option(VITA "Build for Playstation Vita" OFF)
option(PSP "Build for PlayStation Portable" OFF)
option(PS2 "Build for PlayStation 2" OFF)
option(CODE_ANAL "Code analysis" OFF)
set(SUNSHINE_ASSETS_PATH "../SunshineAssets" CACHE STRING "Used only on Vita")
set(STEAMWORKS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/steamworks" CACHE PATH "Path to Steamworks folder")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#code analysis with "include what you use"
if(CODE_ANAL)
	set(CMAKE_C_COMPILER_LAUNCHER "${CMAKE_SOURCE_DIR}/cmake/code_anal.sh")
	set(CMAKE_CXX_COMPILER_LAUNCHER "${CMAKE_SOURCE_DIR}/cmake/code_anal.sh")
endif()

if(STATIC)
	set(ZLIB_USE_STATIC_LIBS ON)
endif()

# For local builds
if(NATIVE)
	check_option(OPTIONS_NATIVE -march=native -mtune=native)
	add_compile_options(${OPTIONS_NATIVE})
endif()

# Use OpenGL ES2
if(USE_OPENGL_ES2)
	add_definitions(-DGLES2_HEADER)
endif()

if(DEVBUILD)
	set(VERSION_STRING "${VERSION_STRING}-dev" FORCE)
endif()
add_definitions(-DVERSION_STRING="${VERSION_STRING}")

# Termux build require some termux specific code
if(TERMUX)
	add_definitions(-DTERMUX)
endif()

if(STEAM)
	add_definitions(-DSTEAM)
	add_subdirectory(steamshim_parent)
	configure_file("${CMAKE_SOURCE_DIR}/steam_appid.txt" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt" COPYONLY)
endif()
