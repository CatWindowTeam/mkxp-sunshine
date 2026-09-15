cmake_minimum_required(VERSION 3.20)
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are disabled. Use: cmake -S . -B build")
endif()
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
project(oneshot)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug
    Release
)

include(FindPackageHandleStandardArgs)
include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)
include(CheckLinkerFlag)
include(CheckIPOSupported)

# Helper for checking if options supported by compiller
function(check_option output_var)
    set(CMAKE_REQUIRED_QUIET TRUE)
    set(result)
    foreach(option IN LISTS ARGN)
        string(MAKE_C_IDENTIFIER "${option}" option_id)
        set(check_var "HAVE_OPTION_${option_id}")
        unset(${check_var} CACHE)

        check_cxx_compiler_flag("${option}" "${check_var}")
        if(${check_var})
            message(STATUS "${option} supported")
            list(APPEND result "${option}")
        else()
			set(check_var_c "HAVE_OPTION_C_${option_id}")
			unset(${check_var_c} CACHE)
        	check_c_compiler_flag("${option}" "${check_var_c}")
        	if(${check_var_c})
        	    message(STATUS "${option} supported")
        	    list(APPEND result "${option}")
        	else()
        	    message(STATUS "${option} unsupported")
        	endif()
        endif()
    endforeach()

    set("${output_var}" "${result}" PARENT_SCOPE)
    set(CMAKE_REQUIRED_QUIET FALSE)
endfunction()

function(check_linker_option output_var)
    set(CMAKE_REQUIRED_QUIET TRUE)
    set(result)

    foreach(option IN LISTS ARGN)
        string(MAKE_C_IDENTIFIER "${option}" option_id)
        set(check_var "HAVE_OPTION_${option_id}")

        check_linker_flag(CXX "${option}" "${check_var}")
        if(${check_var})
            message(STATUS "${option} supported")
            list(APPEND result "${option}")
        else()
			message(STATUS "${option} unsupported")
        endif()
    endforeach()

    set("${output_var}" "${result}" PARENT_SCOPE)
    set(CMAKE_REQUIRED_QUIET FALSE)
endfunction()

# Options
option(STEAM "Build with Steam Support" OFF)
option(SANITIZERS "Enable sanitizers" OFF)
option(NATIVE "Use native instructions,for local use only" OFF)
option(STATIC "Build more static build" OFF)
option(USE_OPENGL_ES2 "GLES2_HEADER define" OFF)
set(VERSION_STRING "0.1.3" CACHE STRING "Version string")
set(BINDING_PATH "binding-mri" CACHE STRING "Binding to use")
set(VITASDK_PATH "/usr/local/vitasdk" CACHE STRING "VitaSDK path")
option(DEVBUILD "Developoment build" OFF)
option(TERMUX "Build for TERMUX" OFF)
option(VITA "Build for PSVita" OFF)
set(SUNSHINE_ASSETS_PATH "../SunshineAssets" CACHE STRING "Used only on Vita")

if(NOT VITA)
    check_ipo_supported(
        RESULT ipo_supported
        OUTPUT ipo_error
        LANGUAGES CXX C
    )
else()
    set(ipo_supported FALSE)
    set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
    add_compile_options(
        -fno-pic
        -fno-PIC
        -fno-pie
        -fno-PIE
    )

    add_link_options(
        -fno-pie
        -no-pie
    )
endif()

#prepare Vita specific thinks
if(VITA)
    if(NOT DEFINED VITASDK_PATH OR VITASDK_PATH STREQUAL "")
        if(DEFINED ENV{VITASDK})
            set(VITASDK_PATH "$ENV{VITASDK}")
        else()
            set(VITASDK_PATH "/usr/local/vitasdk")
        endif()
    endif()
    include("${VITASDK_PATH}/share/vita.cmake" REQUIRED)

    set(VITA_APP_NAME "Oneshot: Sunshine")
    set(VITA_TITLEID "OSS936174")
    set(VITA_VERSION "00.13")

    # SDL3_mixer CMake helper modules
    list(PREPEND CMAKE_MODULE_PATH "${VITASDK_PATH}/SDL_mixer/cmake")
    if(NOT EXISTS "${VITASDK_PATH}/SDL_mixer/cmake/PkgConfigHelper.cmake")
        message(FATAL_ERROR "PkgConfigHelper.cmake not found in ${VITASDK_PATH}/SDL_mixer/cmake")
    endif()
endif()

# Configuration
set(STEAMWORKS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/steamworks" CACHE PATH "Path to Steamworks folder")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# suppress Boost warning
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()

# Make build more static
if(STATIC)
	set(ZLIB_USE_STATIC_LIBS ON)
	set(Boost_USE_STATIC_RUNTIME ON)
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

# Used to indicate if build is experemental/night
if(DEVBUILD)
	add_definitions(-DDEVBUILD)
endif()

# Termux build require some termux specific code
if(TERMUX)
	add_definitions(-DTERMUX)
endif()

# recursive search instead of hardcored list
file(GLOB_RECURSE MAIN_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.h*")
file(GLOB_RECURSE MAIN_SOURCE CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c*")
file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/${BINDING_PATH}/*.h*")
file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/${BINDING_PATH}/*.c*")
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
  NAMES xxd tinyxxd xxd.exe
  REQUIRED
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
	list(APPEND ${outvar} ${outputfile})
endmacro()

foreach(item ${EMBEDDED_INPUT})
	ProcessWithXXD(EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# Platform specific bullshit
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND MAIN_SOURCE assets/resources.rc)
    list(APPEND DEFINES UNICODE)
    list(APPEND PLATFORM_LIBRARIES Secur32 Shlwapi winmm z)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/windows)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND MAIN_HEADERS src/mac-desktop.h)
    list(APPEND MAIN_SOURCE src/mac-desktop.mm)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND cmake -P "${CMAKE_SOURCE_DIR}/patches/mac/CompleteBundle.cmake"
	COMMENT "Executing /patches/mac/CompleteBundle.cmake" VERBATIM)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LINUXPKGS REQUIRED libxfconf-0 libseccomp)
    include_directories(${LINUXPKGS_INCLUDE_DIRS})
    add_compile_options(${LINUXPKGS_CFLAGS_OTHER})
    list(APPEND PLATFORM_LIBRARIES ${LINUXPKGS_LDFLAGS})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    list(APPEND PLATFORM_LIBRARIES log)
    if(TERMUX)
    	find_package(PkgConfig REQUIRED)
    	pkg_check_modules(TERMUXPKGS REQUIRED libxfconf-0)
    	include_directories(${TERMUXPKGS_INCLUDE_DIRS})
    	add_compile_options(${TERMUXPKGS_CFLAGS_OTHER})
    	list(APPEND PLATFORM_LIBRARIES ${TERMUXPKGS_LDFLAGS})
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FREEBSDPKGS REQUIRED libxfconf-0)
    include_directories(${FREEBSDPKGS_INCLUDE_DIRS})
    add_compile_options(${FREEBSDPKGS_CFLAGS_OTHER})
    list(APPEND PLATFORM_LIBRARIES ${FREEBSDPKGS_LDFLAGS})
elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OPENBSDPKGS REQUIRED libxfconf-0)
    include_directories(${OPENBSDPKGS_INCLUDE_DIRS})
    add_compile_options(${OPENBSDPKGS_CFLAGS_OTHER})
    list(APPEND PLATFORM_LIBRARIES ${OPENBSDPKGS_LDFLAGS})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

if(ANDROID)
	if(TERMUX)
		set(APP_TARGET ${PROJECT_NAME})
		add_executable(${APP_TARGET}
                	${MAIN_HEADERS}
                	${MAIN_SOURCE}
                	${BINDING_HEADERS}
                	${BINDING_SOURCE}
                	${EMBEDDED_SOURCE})
	else()
		set(APP_TARGET main)
		add_library(${APP_TARGET} SHARED
                	${MAIN_HEADERS}
                	${MAIN_SOURCE}
                	${BINDING_HEADERS}
                	${BINDING_SOURCE}
                	${EMBEDDED_SOURCE})
	endif()
elseif(VITA)
	set(APP_TARGET ${PROJECT_NAME})
	add_executable(${PROJECT_NAME}
                	${MAIN_HEADERS}
                	${MAIN_SOURCE}
                	${BINDING_HEADERS}
                	${BINDING_SOURCE}
                	${EMBEDDED_SOURCE})	
else()
	set(APP_TARGET ${PROJECT_NAME})
	add_executable(${APP_TARGET} MACOSX_BUNDLE WIN32
		${MAIN_HEADERS}
		${MAIN_SOURCE}
		${BINDING_HEADERS}
		${BINDING_SOURCE}
		${EMBEDDED_SOURCE})
endif()

if(STEAM)
        list(APPEND MAIN_HEADERS steamshim/steamshim_child.h)
        list(APPEND MAIN_SOURCE steamshim/steamshim_child.c)
        add_definitions(-DSTEAM)
        add_subdirectory(steamshim_parent)
        configure_file("${CMAKE_SOURCE_DIR}/steam_appid.txt" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt" COPYONLY)
endif()

# Common arguments on specific CPU arch
if("${ARCH_COMMON}" STREQUAL "")
	message(STATUS "ARCH_COMMON empty")
else()
	check_option(ARCH_COMMON_OPTIONS ${ARCH_COMMON})
	add_compile_options(${ARCH_COMMON_OPTIONS})
endif()

# TODO: fix
if(SANITIZERS)
	# ASan + Ruby = SEGFAULT
	set(SANITIZERS "undefined,leak,shift,shift-base,shift-exponent,integer-divide-by-zero,unreachable,vla-bound,null,signed-integer-overflow,bounds,alignment,object-size,float-divide-by-zero,float-cast-overflow,nonnull-attribute,returns-nonnull-attribute,bool,enum,vptr,pointer-overflow,builtin")
	add_compile_options(-fsanitize=${SANITIZERS})
	add_link_options(-fsanitize=${SANITIZERS})
	check_option(SANITIZERS_OPTIONS -fsanitize-memory-track-origins=2)
	add_compile_options(${SANITIZERS_OPTIONS})
endif()

check_option(OPTIONS_DEBUG
	-g
	-Og)

# Load platfrom specific debug options for compiler from toolchain file
if("${ARCH_DEBUG}" STREQUAL "")
	message(STATUS "ARCH_DEBUG empty")
else()
	check_option(ARCH_DEBUG_OPTIONS ${ARCH_DEBUG})
endif()

check_option(OPTIONS_RELEASE 
	-O3
	-fno-stack-clash-protection
	-fno-unwind-tables
	-fno-asynchronous-unwind-tables
	-fvtable-verify=none
	-fno-ident
	-fstrub=disable
	-fno-harden-compares
	-fno-harden-conditional-branches
	-fno-harden-control-flow-redundancy
	-fno-hardcfr-check-exceptions
	-fno-hardcfr-check-returning-calls
	-fhardcfr-check-noreturn-calls=never
	-fhardcfr-skip-leaf
	-ffunction-sections
	-fdata-sections
	-ftrivial-auto-var-init=uninitialized
	-fzero-call-used-regs=skip
	-fomit-frame-pointer)

# Loading release options specific for cpu arch from toolchain file
if("${ARCH_RELEASE}" STREQUAL "")
	message(STATUS "ARCH_RELEASE empty")
else()
	check_option(ARCH_RELEASE_OPTIONS ${ARCH_RELEASE})
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
check_linker_option(OPTIONS_LINKER_RELEASE
	-Wl,-O3
	-Wl,--gc-sections
	-Wl,--no-copy-dt-needed-entries
	-Wl,--icf=safe
	-Wl,--strip-discarded
	-Wl,--relax
	-Wl,-z,pack-relative-relocs
	-Wl,-z,noexecstack
	-Wl,--build-id=none)
add_link_options("$<$<CONFIG:Release>:${OPTIONS_LINKER_RELEASE}>")

target_compile_definitions(${APP_TARGET} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_options(${APP_TARGET} PRIVATE
    -Wno-unused-parameter
    -Wno-tautological-pointer-compare
    "$<$<CONFIG:Debug>:${OPTIONS_DEBUG}>"
    "$<$<CONFIG:Debug>:${ARCH_DEBUG_OPTIONS}>"
    "$<$<CONFIG:Release>:${OPTIONS_RELEASE}>"
    "$<$<CONFIG:Release>:${OPTIONS_RELEASE_LINUX}>"
    "$<$<CONFIG:Release>:${ARCH_RELEASE_OPTIONS}>"
)



if(CMAKE_BUILD_TYPE STREQUAL "Release")
	if(ipo_supported)
	    set_property(
	        TARGET ${APP_TARGET}
	        PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
	    )
	else()
	    message(WARNING "IPO/LTO is not supported: ${ipo_error}")
	endif()
endif()

if(DEVBUILD)
	target_compile_definitions(${APP_TARGET} PRIVATE DEVBUILD)
endif()
target_compile_definitions(${APP_TARGET} PRIVATE VERSION_STRING="${VERSION_STRING}")
if(TERMUX)
	target_compile_definitions(${APP_TARGET} PRIVATE TERMUX=1)
endif()

find_package(PkgConfig REQUIRED)
find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)
if(NOT TARGET SDL3::SDL3)
	find_package(SDL3 CONFIG REQUIRED)
endif()
if(NOT TARGET SDL3_image::SDL3_image)
	find_package(SDL3_image CONFIG REQUIRED)
endif()
if(NOT TARGET SDL3_mixer::SDL3_mixer)
	find_package(SDL3_mixer CONFIG REQUIRED)
endif()
if(NOT TARGET SDL3_ttf::SDL3_ttf)
	find_package(SDL3_ttf CONFIG REQUIRED)
endif()
if(NOT TARGET SDL3_net::SDL3_net)
	find_package(SDL3_net REQUIRED)
endif()
if(ANDROID)
	find_package(Boost REQUIRED COMPONENTS program_options chrono CONFIG)
else()
	find_package(Boost REQUIRED COMPONENTS program_options chrono)
endif()
find_package(PhysFS)
find_path(PIXMAN_INCLUDE_DIR NAMES pixman.h PATH_SUFFIXES pixman-1)
find_library(PIXMAN_LIBRARY NAMES pixman-1 pixman-1_static pixman-1_staticd)
find_package_handle_standard_args(pixman-1 DEFAULT_MSG PIXMAN_LIBRARY PIXMAN_INCLUDE_DIR)
mark_as_advanced(PIXMAN_INCLUDE_DIR PIXMAN_LIBRARY)
pkg_check_modules(SIGC2 REQUIRED sigc++-2.0)
#find_package(Ruby 3.4 
#	COMPONENTS Interpreter Development
#	REQUIRED)
target_compile_definitions(${APP_TARGET} PRIVATE ${DEFINES})
target_include_directories(${APP_TARGET} PRIVATE
	src
	include
	${SDL_INCLUDE_DIRS}
	${PIXMAN_INCLUDE_DIR}
	${SIGC2_INCLUDE_DIRS}
	${Ruby_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${PHYSFS_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIRS}
	SDL3::Headers
	Boost::headers
	SDL3_mixer::SDL3_mixer
	SDL3_net::SDL3_net)
target_link_directories(${APP_TARGET} PRIVATE ${SIGC2_LIBRARY_DIRS})

target_link_libraries(${APP_TARGET} PRIVATE
	SDL3_image::SDL3_image
	SDL3_mixer::SDL3_mixer
	SDL3_ttf::SDL3_ttf
	SDL3_net::SDL3_net
	SDL3::SDL3
	${PHYSFS_LIBRARY}
	${PIXMAN_LIBRARY}
	${SIGC2_LIBRARIES}
	${PLATFORM_LIBRARIES}
	${Ruby_LIBRARIES}
	Boost::boost
	Boost::chrono
	${Boost_LIBRARIES}
	ZLIB::ZLIB)

if(VITA)
	target_link_libraries(${PROJECT_NAME} PRIVATE webp sharpyuv)
endif()

if(VITA)
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshine.zip
	    COMMENT "Creating sunshine.zip ..."
	    COMMAND mkdir ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshune_zip
	    COMMAND mkdir ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshune_zip/Data
	    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SUNSHINE_ASSETS_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/build/sunshune_zip/"
	    COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/rpgscript.rb ${CMAKE_CURRENT_SOURCE_DIR}/scripts/ ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshune_zip/
	    COMMAND zip -X -9 -r ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshine.zip ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshune_zip/ -x ".git/*" "*/.git/*"
		DEPENDS "${SUNSHINE_ASSETS_PATH}"
	    VERBATIM
	)
	vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME} STRIPPED NOASLR)
	vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
	  VERSION ${VITA_VERSION}
	  NAME ${VITA_APP_NAME}
	  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/icon0.png sce_sys/icon0.png
	  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
	  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
	  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
	  FILE ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshine.zip sunshine.zip
	)
endif()
