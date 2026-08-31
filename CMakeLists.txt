cmake_minimum_required(VERSION 3.20)
project(oneshot)
include(FindPackageHandleStandardArgs)
include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)
include(CheckIPOSupported)

check_ipo_supported(
    RESULT ipo_supported
    OUTPUT ipo_error
    LANGUAGES CXX C
)

# Helper for checking if options supported by compiller
function(check_option output_var)
    set(CMAKE_REQUIRED_QUIET TRUE)
    set(result)

    foreach(option IN LISTS ARGN)
        string(MAKE_C_IDENTIFIER "${option}" option_id)
        set(check_var "HAVE_OPTION_${option_id}")

        check_cxx_compiler_flag("${option}" "${check_var}")
        if(${check_var})
            message(STATUS "${option} supported")
            list(APPEND result "${option}")
        else()
		set(check_var_c "HAVE_OPTION_C_${option_id}")
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


# Options
option(STEAM "Build with Steam Support" OFF)
option(DEBUG "Debug mode" ON)
option(SANITIZERS "Enable sanitizers" OFF)
option(NATIVE "Use native instructions(for local use only)" OFF)
option(STATIC "Build more static build" OFF)
option(USE_OPENGL_ES2 "GLES2_HEADER define" OFF)
set(VERSION_STRING "0.1.2" CACHE STRING "Version string")
option(DEVBUILD "Developoment build" OFF)
option(TERMUX "Build for TERMUX" OFF)

# Configuration
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)
set(STEAMWORKS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/steamworks" CACHE PATH "Path to Steamworks folder")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# shut the fuck up
add_compile_options(-Wno-unused-parameter -Wno-tautological-pointer-compare)

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

# Used to indicate if build is experemental/night
if(DEVBUILD)
	add_definitions(-DDEVBUILD)
endif()

# Termux build require some termux specific code
if(TERMUX)
	add_definitions(-DTERMUX)
endif()

# suppress Boost warning
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()

# recursive search instead of hardcored list
file(GLOB_RECURSE MAIN_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.h*")
file(GLOB_RECURSE MAIN_SOURCE CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c*")

# Platform specific bullshit
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND MAIN_SOURCE assets/resources.rc)
    list(APPEND DEFINES UNICODE)
    list(APPEND PLATFORM_LIBRARIES Secur32 Shlwapi winmm z)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/windows)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND MAIN_HEADERS src/mac-desktop.h)
    list(APPEND MAIN_SOURCE src/mac-desktop.mm)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cmake -P "${CMAKE_SOURCE_DIR}/patches/mac/CompleteBundle.cmake"
        VERBATIM
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LINUXPKGS REQUIRED gtk+-3.0 libxfconf-0 libseccomp)
    include_directories(${LINUXPKGS_INCLUDE_DIRS})
    add_compile_options(${LINUXPKGS_CFLAGS_OTHER})
    list(APPEND PLATFORM_LIBRARIES ${LINUXPKGS_LDFLAGS})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    if(TERMUX)
    	find_package(PkgConfig REQUIRED)
    	pkg_check_modules(TERMUXPKGS REQUIRED gtk+-3.0 libxfconf-0)
    	include_directories(${TERMUXPKGS_INCLUDE_DIRS})
    	add_compile_options(${TERMUXPKGS_CFLAGS_OTHER})
    	list(APPEND PLATFORM_LIBRARIES ${TERMUXPKGS_LDFLAGS})
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FREEBSDPKGS REQUIRED gtk+-3.0 libxfconf-0)
    include_directories(${FREEBSDPKGS_INCLUDE_DIRS})
    add_compile_options(${FREEBSDPKGS_CFLAGS_OTHER})
    list(APPEND PLATFORM_LIBRARIES ${FREEBSDPKGS_LDFLAGS})
    set(STEAM OFF)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(STEAM OFF)
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

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

# recursive search insead of hardcored list
file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/binding-mri/*.h*")
file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/binding-mri/*.c*")

if(STEAM)
        list(APPEND MAIN_HEADERS
                steamshim/steamshim_child.h
        )
        list(APPEND MAIN_SOURCE
                steamshim/steamshim_child.c
        )
        add_definitions(-DSTEAM)
        add_subdirectory(steamshim_parent)
        # steam_appid.txt
        configure_file(
                "${CMAKE_SOURCE_DIR}/steam_appid.txt"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt"
                COPYONLY)
endif()

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
	set(SANITIZERS "undefined,leak,shift,shift-base,shift-exponent,integer-divide-by-zero,unreachable,vla-bound,null,signed-integer-overflow,bounds,alignment,object-size,float-divide-by-zero,float-cast-overflow,nonnull-attribute,returns-nonnull-attribute,bool,enum,vptr,pointer-overflow,builtin")
	add_compile_options(-fsanitize=${SANITIZERS})
	add_link_options(-fsanitize=${SANITIZERS})
        check_option(SANITIZERS_OPTIONS -fsanitize-memory-track-origins=2)
        add_compile_options(${SANITIZERS_OPTIONS})
endif()

if(DEBUG)
	check_option(OPTIONS_DEBUG
		-g3
		-fno-omit-frame-pointer
		-Og
		-fcheck-new
		-fstrict-flex-arrays=3
		-fprofile
		-fno-optimize-sibling-calls
		-Wall
		-Wextra
		-ftemplate-backtrace-limit=0
		-fdiagnostics-show-template-tree
		-fopt-info-all)
	add_compile_options(${OPTIONS_DEBUG})

	# Load platfrom specific debug options for compiler from toolchain file
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
	-fdata-sections
	-fvirtual-function-elimination
	-fstdarg-opt
	-frename-registers
	-fuse-linker-plugin
	-fipa-pta
	-fipa-icf
	-fext-dce
	-fstrict-enums
	-fimplicit-constexpr
	-fomit-frame-pointer
	-fno-ident
	-fvirtual-function-elimination
	-fwhole-program-vtables
	-fweb)

	add_compile_options(${OPTIONS_RELEASE})

	add_link_options(-Wl,-O2
	-Wl,--gc-sections
	-Wl,--as-needed
	-Wl,--no-undefined
	-Wl,--strip-all
	-Wl,--no-copy-dt-needed-entries)

	#Platform Specific
	if(LINUX)
		check_option(OPTIONS_RELEASE_LINUX -fno-plt)
		add_compile_options(${OPTIONS_RELEASE_LINUX})
	endif()

	# Loading release options specific for cpu arch from toolchain file
	if("${ARCH_RELEASE}" STREQUAL "")
		message(STATUS "ARCH_RELEASE empty")
	else()
	  	check_option(ARCH_RELEASE_OPTIONS ${ARCH_RELEASE})
	  	add_compile_options(${ARCH_RELEASE_OPTIONS})
	endif()
endif()

if(ANDROID)
	if(TERMUX)
		add_executable(${PROJECT_NAME}
                	${MAIN_HEADERS}
                	${MAIN_SOURCE}
                	${BINDING_HEADERS}
                	${BINDING_SOURCE}
                	${EMBEDDED_SOURCE})
	else()
		add_library(${PROJECT_NAME} SHARED
                	${MAIN_HEADERS}
                	${MAIN_SOURCE}
                	${BINDING_HEADERS}
                	${BINDING_SOURCE}
                	${EMBEDDED_SOURCE})
	endif()
else()
	add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32
		${MAIN_HEADERS}
		${MAIN_SOURCE}
		${BINDING_HEADERS}
		${BINDING_SOURCE}
		${EMBEDDED_SOURCE})
endif()

if(NOT DEBUG)
	if(ipo_supported)
	    set_property(
	        TARGET ${PROJECT_NAME}
	        PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
	    )
	else()
	    message(WARNING "IPO/LTO is not supported: ${ipo_error}")
	endif()
endif()

# Used when creating crashdump or on debug text in main menu script
target_compile_definitions(${PROJECT_NAME} PRIVATE VERSION_STRING="${VERSION_STRING}")

find_package(PkgConfig REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_mixer CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(SDL3_net REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options chrono)
find_package(PhysFS REQUIRED)
find_path(PIXMAN_INCLUDE_DIR NAMES pixman.h PATH_SUFFIXES pixman-1)
find_library(PIXMAN_LIBRARY NAMES pixman-1 pixman-1_static pixman-1_staticd)
find_package(Ruby 3.0 REQUIRED COMPONENTS Development)
find_package_handle_standard_args(pixman-1 DEFAULT_MSG PIXMAN_LIBRARY PIXMAN_INCLUDE_DIR)
mark_as_advanced(PIXMAN_INCLUDE_DIR PIXMAN_LIBRARY)
pkg_check_modules(SIGC2 REQUIRED sigc++-2.0)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFINES})

target_include_directories(${PROJECT_NAME} PRIVATE
	src
	include
	${PIXMAN_INCLUDE_DIR}
	${SIGC2_INCLUDE_DIRS}
	${Ruby_INCLUDE_DIRS}
	Boost::headers
	${ZLIB_INCLUDE_DIRS}
	SDL3_mixer::SDL3_mixer
	SDL3_net::SDL3_net)

target_link_libraries(${PROJECT_NAME} PRIVATE
	SDL3::SDL3
	SDL3_image::SDL3_image
	SDL3_mixer::SDL3_mixer
	SDL3_ttf::SDL3_ttf
	SDL3_net::SDL3_net
	physfs
	${PIXMAN_LIBRARY}
	${SIGC2_LIBRARIES}
	${PLATFORM_LIBRARIES}
	${Ruby_LIBRARIES}
	Boost::boost
	Boost::chrono
	${Boost_LIBRARIES}
	ZLIB::ZLIB
	${PLATFORM_LIBS})

add_subdirectory(scripts)
