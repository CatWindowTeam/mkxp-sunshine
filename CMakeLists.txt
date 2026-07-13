cmake_minimum_required(VERSION 3.28.3)
project(oneshot)
include(FindPackageHandleStandardArgs)

# Options
option(STEAM "Build for Steam" OFF)
option(DEBUG "Debug mode" ON)
option(NATIVE "Use native instructions(for local use only)" OFF)
option(MODLOADER "Build with modloader" ON)

#Configuration
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)
set(VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION "5.1.2600.0")
set(STEAMWORKS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/steamworks" CACHE PATH "Path to Steamworks folder")
set(ZLIB_USE_STATIC_LIBS ON)
set(OPENSSL_USE_STATIC_LIBS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 23)

# main stuff
set(MAIN_HEADERS
	src/quadarray.h
	src/audio.h
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
	src/al-util.h
	src/boost-hash.h
	src/debugwriter.h
	src/gl-fun.h
	src/gl-meta.h
	src/vertex.h
	src/soundemitter.h
	src/security.h
	src/aldatasource.h
	src/alstream.h
	src/audiostream.h
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
)

set(MAIN_SOURCE
	src/main.cpp
	src/audio.cpp
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
	src/soundemitter.cpp
	src/sdlsoundsource.cpp
	src/alstream.cpp
	src/audiostream.cpp
	src/rgssad.cpp
	src/vorbissource.cpp
	src/oneshot.cpp
	src/screen.cpp
	src/i18n.cpp
	src/meow.cpp
	src/modloader.cpp
	src/sunshine.cpp
	src/lightmap.cpp
)

if(WIN32)
	list(APPEND MAIN_SOURCE assets/resources.rc)
	list(APPEND DEFINES UNICODE)
	list(APPEND PLATFORM_LIBRARIES Secur32 Shlwapi winmm z)
	include_directories( ${CMAKE_CURRENT_BINARY_DIR}/windows )
elseif(APPLE)
	list(APPEND MAIN_HEADERS src/mac-desktop.h)
	list(APPEND MAIN_SOURCE src/mac-desktop.mm)
elseif(LINUX)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(LINUXPKGS REQUIRED gtk+-3.0 libxfconf-0 libseccomp)
	include_directories(${LINUXPKGS_INCLUDE_DIRS})
	add_compile_options(${LINUXPKGS_CFLAGS_OTHER})
	list(APPEND PLATFORM_LIBRARIES ${LINUXPKGS_LDFLAGS})
	list(APPEND MAIN_SOURCE src/xdg-user-dir-lookup.c)
	list(APPEND MAIN_HEADERS src/xdg-user-dir-lookup.h)
elseif(BSD)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LINUXPKGS REQUIRED gtk+-3.0)
    include_directories(${LINUXPKGS_INCLUDE_DIRS})
    add_compile_options(${LINUXPKGS_CFLAGS_OTHER})
    list(APPEND PLATFORM_LIBRARIES ${LINUXPKGS_LDFLAGS})
    list(APPEND MAIN_SOURCE src/xdg-user-dir-lookup.c)
    list(APPEND MAIN_HEADERS src/xdg-user-dir-lookup.h)
	set(STEAM OFF)
	set(MODLOADER OFF)
elseif(EMSCRIPTEN)
	set(STEAM OFF)
	set(MODLOADER OFF)
  	set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
elseif(APPLE)
  		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND cmake -P "${CMAKE_SOURCE_DIR}/patches/mac/CompleteBundle.cmake" VERBATIM)
endif()

if (STEAM)
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
endif()

source_group("MKXP Source" FILES ${MAIN_SOURCE} ${MAIN_HEADERS})

## Setup embedded source ##

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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/xxdhack)
include_directories(
	${CMAKE_CURRENT_BINARY_DIR}/xxdhack
)

foreach(item ${EMBEDDED_INPUT})
	ProcessWithXXD(EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

source_group("Embedded Source" FILES ${EMBEDDED_INPUT} ${EMBEDDED_SOURCE})

## Setup binding source ##
list(APPEND DEFINES
	BINDING_MRI
)
set(BINDING_HEADERS
	binding-mri/binding-util.h
	binding-mri/binding-types.h
	binding-mri/serializable-binding.h
	binding-mri/disposable-binding.h
	binding-mri/sceneelement-binding.h
	binding-mri/viewportelement-binding.h
	binding-mri/flashable-binding.h
	binding-mri/keybindings-binding.h
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
)

source_group("Binding Source" FILES ${BINDING_SOURCE} ${BINDING_HEADERS})



################ 
# Настройки зависимые от компилятора
if (MSVC)
	list(APPEND DEFINES _CRT_SECURE_NO_WARNINGS NOMINMAX )
	if(DEBUG)
		add_compile_options(/EHr /fsanitize /validate-charset /DEBUG /RTCu /RTCs /RTCc)
	else()
		add_compile_options(/O2 /fp:fast /GL /GF /GA)
	endif()
else()
	if(NATIVE)
	    add_compile_options(-march=native -mtune=native)
	endif()
	if(DEBUG)
		add_compile_options(-g3 -fno-omit-frame-pointer)
		#add_compile_options(-g3 -O0 -fno-omit-frame-pointer -ggdb -fcheck-new -gbtf)
	else()
		add_compile_options(-O2 -ffast-math -fipa-pta -ftree-vectorize -fstdarg-opt -fomit-frame-pointer -frename-registers 
		-ffunction-sections -fdata-sections -fvisibility=hidden)
		add_link_options(-Wl,-O2 -Wl,--gc-sections -Wl,--as-needed -Wl,--strip-debug  -Wl,--no-undefined)
		if(!WIN32)
			add_compile_options(-flto)
		endif()
	endif()
endif()

###############

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

find_package(PkgConfig REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SDL3 CONFIG)
find_package(SDL3_image CONFIG)
find_package(SDL3_sound CONFIG)
find_package(SDL3_ttf CONFIG)
find_package(OpenAL CONFIG)
find_package(Boost REQUIRED COMPONENTS program_options chrono)
find_package(PhysFS CONFIG)
find_path(PIXMAN_INCLUDE_DIR NAMES pixman.h PATH_SUFFIXES pixman-1)
find_library(PIXMAN_LIBRARY NAMES pixman-1 pixman-1_static pixman-1_staticd)
find_package(Ruby 3.0 REQUIRED COMPONENTS Development)
pkg_check_modules(VORBISFILE REQUIRED vorbisfile)
find_package_handle_standard_args(pixman-1 DEFAULT_MSG PIXMAN_LIBRARY PIXMAN_INCLUDE_DIR)
mark_as_advanced(PIXMAN_INCLUDE_DIR PIXMAN_LIBRARY)
if(MODLOADER)
	find_package(OpenSSL CONFIG REQUIRED COMPONENTS Crypto)
	pkg_check_modules(SIGC2 REQUIRED sigc++-2.0)
	pkg_check_modules(LIBZIP REQUIRED libzip)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFINES})

target_include_directories(${PROJECT_NAME} PRIVATE src include ${PIXMAN_INCLUDE_DIR} ${SIGC2_INCLUDE_DIRS}
${Ruby_INCLUDE_DIRS} Boost::boost ${Boost_INCLUDE_DIRS} 
${VORBISFILE_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image 
SDL3_sound::SDL3_sound SDL3_ttf::SDL3_ttf OpenAL::OpenAL
physfs ${PIXMAN_LIBRARY} ${SIGC2_LIBRARIES} ${PLATFORM_LIBRARIES} 
${Ruby_LIBRARIES} Boost::boost Boost::chrono ${Boost_LIBRARIES}
${VORBISFILE_LIBRARIES} ZLIB::ZLIB ${PLATFORM_LIBS})

if(MODLOADER)
	target_include_directories(${PROJECT_NAME} PRIVATE ${LIBZIP_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBZIP_LIBRARIES} OpenSSL::Crypto)
else()

endif()

if(STEAM)
	add_subdirectory(steamshim_parent)

	# steam_appid.txt
	configure_file(
		"${CMAKE_SOURCE_DIR}/steam_appid.txt"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt"
		COPYONLY)
endif()

add_subdirectory(scripts)
