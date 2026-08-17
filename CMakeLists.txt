cmake_minimum_required(VERSION 3.28.3)
project(oneshot)
include(FindPackageHandleStandardArgs)
include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)
# Helper for checking if options supported
function(check_option var a)
  set(all ${a})
  set(var "")
  list(APPEND all ${ARGN})
  
  foreach(x IN LISTS all)
    check_cxx_compiler_flag(${x} HAVE_OPTION)  
    if(HAVE_OPTION)
		message(STATUS "${x} SUPPORTED")
		list(APPEND var ${x})
	else()
		check_c_compiler_flag(${x} HAVE_OPTION_C)
		if(HAVE_OPTION_C)
			message(STATUS "${x} SUPPORTED")
			list(APPEND var ${x})
		else()
			message(STATUS "${x} UNSUPPORTED")
		endif()
    endif()
    
  endforeach()  
  set(${var} "${var}" PARENT_SCOPE)
endfunction()

# Options
option(STEAM "Build for Steam" OFF)
option(DEBUG "Debug mode" ON)
option(SANITIZERS "Enable sanitizers" OFF)
option(NATIVE "Use native instructions(for local use only)" OFF)
option(STATIC "Build more static build" OFF)
option(USE_OPENGL_ES2 "GLES2_HEADER define" OFF)
set(VERSION_STRING "0.1.2" CACHE STRING "Version string")
option(DEVBUILD "Developoment build" OFF)

# Configuration
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)
set(STEAMWORKS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/steamworks" CACHE PATH "Path to Steamworks folder")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 23)
add_compile_options(-Wno-unused-parameter)

# Make build more static
if(STATIC)
	check_option(OPTIONS_STATIC -static-libgcc -static-libstdc++)
	add_compile_options(${OPTIONS_STATIC})
	set(ZLIB_USE_STATIC_LIBS ON)
endif()

# Use OpenGL ES2
if(USE_OPENGL_ES2)
	add_definitions(-DGLES2_HEADER)
endif()

if(POLICY CMP0167) # suppress Boost warning
    cmake_policy(SET CMP0167 OLD)
endif()

# main stuff
set(MAIN_HEADERS
	src/quadarray.h
	src/binding.h
	src/bitmap.h
	src/disposable.h
	src/etc.h
	src/etc-internal.h
	src/eventthread.h
	src/flashable.h
	src/font.h
	src/input.h
	src/plane.h
	src/scene.h
	src/sprite.h
	src/table.h
	src/texpool.h
	src/tilequad.h
	src/transform.h
	src/viewport.h
	src/window.h
	src/serializable.h
	src/shader.h
	src/glstate.h
	src/quad.h
	src/tilemap.h
	src/tilemap-common.h
	src/graphics.h
	src/gl-debug.h
	src/global-ibo.h
	src/exception.h
	src/filesystem.h
	src/serial-util.h
	src/intrulist.h
	src/gl-util.h
	src/util.h
	src/config.h
	src/keybindings.h
	src/tileatlas.h
	src/sharedstate.h
	src/boost-hash.h
	src/debugwriter.h
	src/gl-fun.h
	src/gl-meta.h
	src/vertex.h
	src/security.h
	src/rgssad.h
	src/sdl-util.h
	src/oneshot.h
	src/pipe.h
	src/i18n.h
	src/meow.h
	src/modloader.h
	src/sunshine.h
	src/lightmap.h
	src/define.h
	src/signals/signal.h
	src/signals/signalconnection.h
	src/signals/rubydispatcher.h
	src/signals/maindispatcher.h
	src/sound/audio.h
	src/sound/audiogroup.h
	src/sound/audioplayback.h
	src/sound/audiosource.h
	src/sound/audioutil.h
)

set(MAIN_SOURCE
	src/main.cpp
	src/bitmap.cpp
	src/eventthread.cpp
	src/filesystem.cpp
	src/font.cpp
	src/input.cpp
	src/plane.cpp
	src/scene.cpp
	src/sprite.cpp
	src/table.cpp
	src/tilequad.cpp
	src/viewport.cpp
	src/window.cpp
	src/texpool.cpp
	src/shader.cpp
	src/glstate.cpp
	src/tilemap.cpp
	src/autotiles.cpp
	src/graphics.cpp
	src/gl-debug.cpp
	src/etc.cpp
	src/config.cpp
	src/security.cpp
	src/keybindings.cpp
	src/tileatlas.cpp
	src/sharedstate.cpp
	src/gl-fun.cpp
	src/gl-meta.cpp
	src/vertex.cpp
	src/rgssad.cpp
	src/oneshot.cpp
	src/screen.cpp
	src/i18n.cpp
	src/meow.cpp
	src/modloader.cpp
	src/sunshine.cpp
	src/lightmap.cpp
	src/signals/rubydispatcher.cpp
	src/signals/maindispatcher.cpp
	src/sound/audio.cpp
	src/sound/audiogroup.cpp
	src/sound/audioplayback.cpp
	src/sound/audiosource.cpp
)

# Platform specific bullshit
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND MAIN_SOURCE assets/resources.rc)
    list(APPEND DEFINES UNICODE)
    list(APPEND PLATFORM_LIBRARIES Secur32 Shlwapi winmm z)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/windows)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND MAIN_HEADERS src/mac-desktop.h)
    list(APPEND MAIN_SOURCE src/mac-desktop.mm)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LINUXPKGS REQUIRED gtk+-3.0 libxfconf-0 libseccomp)
    include_directories(${LINUXPKGS_INCLUDE_DIRS})
    add_compile_options(${LINUXPKGS_CFLAGS_OTHER})
    list(APPEND PLATFORM_LIBRARIES ${LINUXPKGS_LDFLAGS})
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LINUXPKGS REQUIRED gtk+-3.0 libxfconf-0)
    include_directories(${LINUXPKGS_INCLUDE_DIRS})
    add_compile_options(${LINUXPKGS_CFLAGS_OTHER})
    list(APPEND PLATFORM_LIBRARIES ${LINUXPKGS_LDFLAGS})
    set(STEAM OFF)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(STEAM OFF)
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
elseif(APPLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cmake -P "${CMAKE_SOURCE_DIR}/patches/mac/CompleteBundle.cmake"
        VERBATIM
    )
endif()

source_group("MKXP Source" FILES ${MAIN_SOURCE} ${MAIN_HEADERS})
set(EMBEDDED_INPUT
	shader/common.h
	shader/transSimple.frag
	shader/trans.frag
	shader/hue.frag
	shader/sprite.frag
	shader/worldMachine.frag
	shader/water.frag
	shader/crt.frag
	shader/tilemapWater.frag
	shader/plane.frag
	shader/gray.frag
	shader/bitmapBlit.frag
	shader/flatColor.frag
	shader/simple.frag
	shader/simpleColor.frag
	shader/simpleAlpha.frag
	shader/simpleAlphaUni.frag
	shader/dynamicLight.frag
	shader/flashMap.frag
	shader/obscured.frag
	shader/minimal.vert
	shader/simple.vert
	shader/simpleColor.vert
	shader/sprite.vert
	shader/tilemap.vert
	shader/blur.frag
	shader/blurH.vert
	shader/blurV.vert
	shader/simpleMatrix.vert
	assets/icon.png
	assets/gamecontrollerdb.txt
	assets/crash.png
	assets/the_modded_machine.png
	binding-mri/module_rpg1.rb
)

## Process Embeddeds ##
find_program(XXD_EXE
  NAMES xxd xxd.exe
  DOC "Location of the xxd executable"
  HINTS
    ENV PATH
    /usr/bin /usr/local/bin .
)

macro(ProcessWithXXD outvar inputfile outdir)
	get_filename_component(basefile ${inputfile} NAME)
	set(outputfile ${outdir}/${basefile}.xxd)
	set_source_files_properties(${outputfile} PROPERTIES HEADER_FILE_ONLY TRUE)
	add_custom_command(
		OUTPUT ${outputfile}
		COMMAND ${XXD_EXE} -i ${inputfile} ${outputfile}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DEPENDS ${inputfile}
		COMMENT "Generating XXD for ${inputfile}"
	)
	list(APPEND ${outvar}
		${outputfile}
	)
endmacro()

foreach(item ${EMBEDDED_INPUT})
	ProcessWithXXD(EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

source_group("Embedded Source" FILES ${EMBEDDED_INPUT} ${EMBEDDED_SOURCE})
set(BINDING_HEADERS
	binding-mri/binding-util.h
	binding-mri/binding-types.h
	binding-mri/serializable-binding.h
	binding-mri/disposable-binding.h
	binding-mri/sceneelement-binding.h
	binding-mri/viewportelement-binding.h
	binding-mri/flashable-binding.h
	binding-mri/keybindings-binding.h
	binding-mri/signalconnection-binding.h
	binding-mri/audioplayback-binding.h
)
set(BINDING_SOURCE
	binding-mri/binding-mri.cpp
	binding-mri/binding-util.cpp
	binding-mri/table-binding.cpp
	binding-mri/etc-binding.cpp
	binding-mri/bitmap-binding.cpp
	binding-mri/font-binding.cpp
	binding-mri/graphics-binding.cpp
	binding-mri/input-binding.cpp
	binding-mri/sprite-binding.cpp
	binding-mri/viewport-binding.cpp
	binding-mri/plane-binding.cpp
	binding-mri/window-binding.cpp
	binding-mri/tilemap-binding.cpp
	binding-mri/audio-binding.cpp
	binding-mri/module_rpg.cpp
	binding-mri/filesystem-binding.cpp
	binding-mri/oneshot-binding.cpp
    binding-mri/sunshine-binding.cpp
	binding-mri/steam-binding.cpp
	binding-mri/wallpaper-binding.cpp
	binding-mri/journal-binding.cpp
	binding-mri/niko-binding.cpp
	binding-mri/time-binding.cpp
	binding-mri/shader-binging.cpp
	binding-mri/modloader-binding.cpp
	binding-mri/keybindings-binding.cpp
	binding-mri/lightmap-binding.cpp
	binding-mri/signalconnection-binding.cpp
	binding-mri/gamepad-binding.cpp
	binding-mri/profiler-binding.cpp
	binding-mri/audioplayback-binding.cpp
)

source_group("Binding Source" FILES ${BINDING_SOURCE} ${BINDING_HEADERS})
if(NATIVE)
	check_option(OPTIONS_NATIVE -march=native -mtune=native)
	add_compile_options(${OPTIONS_NATIVE})
endif()

# Common arguments on specific CPU arch
if("${ARCH_COMMON}" STREQUAL "")
	message(STATUS "ARCH_COMMON empty")
else()
  check_option(ARCH_COMMON_OPTIONS ${ARCH_COMMON})
  add_compile_options(${ARCH_COMMON_OPTIONS})
endif()

if(SANITIZERS)
	# ASan + Ruby = SEGFAULT
	set(SANITIZERS "undefined,leak,shift,shift-base,shift-exponent,integer-divide-by-zero,unreachable,vla-bound,null,signed-integer-overflow,bounds,bounds-strict,alignment,object-size,float-divide-by-zero,float-cast-overflow,nonnull-attribute,returns-nonnull-attribute,bool,enum,vptr,pointer-overflow,builtin,pointer-subtract,pointer-compare")
	check_option(SANITIZERS_OPTIONS -fsanitize=${SANITIZERS})
	add_compile_options(${SANITIZERS_OPTIONS})
	add_link_options(${SANITIZERS_OPTIONS})
endif()
if(DEBUG)
	check_option(OPTIONS_DEBUG
		-g3
		-fno-omit-frame-pointer
		-Og
		-fcheck-new)
	add_compile_options(${OPTIONS_DEBUG})

	if("${ARCH_DEBUG}" STREQUAL "")
		message(STATUS "ARCH_DEBUG empty")
	else()
	  	check_option(ARCH_DEBUG_OPTIONS ${ARCH_DEBUG})
	  	add_compile_options(${ARCH_DEBUG_OPTIONS})
	endif()
else()
	check_option(OPTIONS_RELEASE -O3
	-ffast-math
	-fvisibility=hidden
	-fno-math-errno
	-fno-trapping-math
	-ffunction-sections
	-fsection-anchors
	-fdata-sections
	-fvirtual-function-elimination
	-fstdarg-opt
	-frename-registers
	-fuse-linker-plugin
	-fipa-pta
	-fext-dce)
	add_compile_options(${OPTIONS_RELEASE})
		
	add_link_options(-Wl,-O2 
	-Wl,--gc-sections 
	-Wl,--as-needed  
	-Wl,--no-undefined
	-Wl,--strip-all
	-Wl,--no-copy-dt-needed-entries)
	if(NOT WIN32)
		check_option(OPTIONS_RELEASE_WIN32_FLTO -flto=thin)
		add_compile_options(${OPTIONS_RELEASE_WIN32_FLTO})
		add_link_options(${OPTIONS_RELEASE_WIN32_FLTO})
	endif()
	if("${ARCH_RELEASE}" STREQUAL "")
		message(STATUS "ARCH_RELEASE empty")
	else()
	  	check_option(ARCH_RELEASE_OPTIONS ${ARCH_RELEASE})
	  	add_compile_options(${ARCH_RELEASE_OPTIONS})
	endif()
endif()

if(ANDROID)
	add_library(main SHARED
		${MAIN_HEADERS}
		${MAIN_SOURCE}
		${BINDING_HEADERS}
		${BINDING_SOURCE}
		${EMBEDDED_SOURCE}
	)
else()
	add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32
		${MAIN_HEADERS}
		${MAIN_SOURCE}
		${BINDING_HEADERS}
		${BINDING_SOURCE}
		${EMBEDDED_SOURCE}
	)
endif()

if(DEVBUILD)
	target_compile_definitions(${PROJECT_NAME} PRIVATE DEVBUILD)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE VERSION_STRING="${VERSION_STRING}")

find_package(PkgConfig REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SDL3 CONFIG)
find_package(SDL3_image CONFIG)
find_package(SDL3_mixer CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG)
find_package(Boost REQUIRED COMPONENTS program_options chrono)
find_package(PhysFS)
find_path(PIXMAN_INCLUDE_DIR NAMES pixman.h PATH_SUFFIXES pixman-1)
find_library(PIXMAN_LIBRARY NAMES pixman-1 pixman-1_static pixman-1_staticd)
find_package(Ruby 3.0 REQUIRED COMPONENTS Development)
find_package_handle_standard_args(pixman-1 DEFAULT_MSG PIXMAN_LIBRARY PIXMAN_INCLUDE_DIR)
mark_as_advanced(PIXMAN_INCLUDE_DIR PIXMAN_LIBRARY)
pkg_check_modules(SIGC2 REQUIRED sigc++-2.0)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFINES})

target_include_directories(${PROJECT_NAME} PRIVATE src include ${PIXMAN_INCLUDE_DIR} ${SIGC2_INCLUDE_DIRS}
${Ruby_INCLUDE_DIRS} Boost::boost ${Boost_INCLUDE_DIRS} 
${ZLIB_INCLUDE_DIRS} SDL3_mixer::SDL3_mixer)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image 
SDL3_mixer::SDL3_mixer SDL3_ttf::SDL3_ttf
physfs ${PIXMAN_LIBRARY} ${SIGC2_LIBRARIES} ${PLATFORM_LIBRARIES} 
${Ruby_LIBRARIES} Boost::boost Boost::chrono ${Boost_LIBRARIES}
ZLIB::ZLIB ${PLATFORM_LIBS})

if(STEAM)
	list(APPEND MAIN_HEADERS
		src/steam.h
		steamshim/steamshim_child.h
	)
	list(APPEND MAIN_SOURCE
		src/steam.cpp
		steamshim/steamshim_child.c
	)
	list(APPEND DEFINES
		STEAM
	)
	add_subdirectory(steamshim_parent)
	# steam_appid.txt
	configure_file(
		"${CMAKE_SOURCE_DIR}/steam_appid.txt"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt"
		COPYONLY)
endif()
add_subdirectory(scripts)
