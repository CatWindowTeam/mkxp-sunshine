include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/check.cmake)

macro(option_with_define NAME DESC DEFAULT_STATE)
	option(${NAME} "${DESC}" ${DEFAULT_STATE})
	if(${NAME})
		add_definitions("-D${NAME}")
	endif()
endmacro()

# Options
option(NATIVE "Use native instructions,for local use only" OFF)
option_with_define(RENDER_GL_MKXP_OPENGL_ES2 "In gl-mkxp, use OpenGLL ES2 isntead of Desktop OpenGL" OFF)
set(VERSION_STRING "0.2.0" CACHE STRING "Version string")
set(BINDING "cruby" CACHE STRING "Binding to use")
set(VITASDK_PATH "/usr/local/vitasdk" CACHE STRING "VitaSDK path")
option_with_define(TERMUX "Build for TERMUX" OFF)
option(LTO "Use LTO optimization" ON)
option(VITA "Build for Playstation Vita" OFF)
option(CODE_ANAL "Code analysis" OFF)
option(MRUBY_AUTOBUILD "Build mruby during cmake configuration" ON)
set(SUNSHINE_ASSETS_PATH "SunshineAssets" CACHE STRING "Path to SunshineAssets")
option(API_ONESHOT_EXTENSIONS_XFCE "Support for wallpaper setter for xfce4, libxfconf required " ON)
option_with_define(RENDER_SIMPLE "Build with simple render backend" ON)
option_with_define(RENDER_GPU "Build with GPU render backend" ON)
option_with_define(RENDER_GL_MKXP "Build with gl-mkxp render backend" ON)
option_with_define(UNSAFE_OPTIMIZATIONS "Allow unsafe optimizations" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#code analysis
if(CODE_ANAL)
	#tools
	check_option(WARNS -Wdouble-promotion
			-Wduplicate-decl-specifier
			-Wformat=2
			-Wdisabled-optimization
			-Wunused-macros
			-Wunsafe-loop-optimizations
			-Winline
			-Waggressive-loop-optimizations
			-Wstrict-overflow=2
			-Wvector-operation-performance
			-Wpass-failed
			-Wloop-analysis
			-Wrange-loop-analysis
			-Wrange-loop-construct
			-Wrange-loop-bind-reference
			-Wlarge-by-value-copy
			-Wmove
			-Wunused
			-Wunreachable-code
			-Wunreachable-code-aggressive
			-Wunreachable-code-break
			-Wunreachable-code-loop-increment
			-Wunreachable-code-return)
	add_compile_options(${WARNS})
	set(CMAKE_LINK_WHAT_YOU_USE ON)
endif()

# For local builds
if(NATIVE)
	check_option(OPTIONS_NATIVE -march=native -mtune=native)
	add_compile_options(${OPTIONS_NATIVE})
endif()

add_definitions(-DVERSION_STRING="${VERSION_STRING}")
