cmake_minimum_required(VERSION 3.20)
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are disabled. Use: cmake -S . -B build")
endif()

project(_______ C)
include(FindPackageHandleStandardArgs)
include(CheckCCompilerFlag)
include(CheckLinkerFlag)

# Process Embeddeds
find_program(XXD_EXE
  NAMES xxd xxd.exe
  DOC "Location of the xxd executable"
  HINTS
    ENV PATH
    /usr/bin /usr/local/bin . ..
)


# PSVita shit #1
option(VITA "Build for Playstation Vita" OFF)
if(VITA)
    if(NOT DEFINED VITASDK_PATH OR VITASDK_PATH STREQUAL "")
        if(DEFINED ENV{VITASDK})
            set(VITASDK_PATH "$ENV{VITASDK}")
        else()
            set(VITASDK_PATH "/usr/local/vitasdk")
        endif()
    endif()
    include("${VITASDK_PATH}/share/vita.cmake" REQUIRED)
endif()


# Helper for checking if options supported by compiller
function(check_option output_var)
    set(CMAKE_REQUIRED_QUIET TRUE)
    set(result)
    foreach(option IN LISTS ARGN)
        string(MAKE_C_IDENTIFIER "${option}" option_id)
        set(check_var "HAVE_OPTION_${option_id}")
        unset(${check_var} CACHE)
        check_c_compiler_flag("${option}" "${check_var}")
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


function(check_linker_option output_var)
    set(CMAKE_REQUIRED_QUIET TRUE)
    set(result)
    foreach(option IN LISTS ARGN)
        string(MAKE_C_IDENTIFIER "${option}" option_id)
        set(check_var "HAVE_OPTION_${option_id}")
        check_linker_flag(C "${option}" "${check_var}")
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


set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SUNSHINE_ASSETS_PATH "../SunshineAssets" CACHE STRING "Used only on Vita")
set(EMBEDDED_INPUT assets/icon.png assets/default.png)

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

add_executable(${PROJECT_NAME} ${EMBEDDED_SOURCE} main.c)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_net CONFIG REQUIRED)
if(VITA)
	set(PLATFORM_LIBRARIES webp sharpyuv pthread)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image-static SDL3_net::SDL3_net-static SDL3::SDL3-static ${PLATFORM_LIBRARIES})
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_options(${PROJECT_NAME} PRIVATE -mwindows -municode)
	list(APPEND DEFINES UNICODE)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFINES})

check_option(OPTIONS_RELEASE
	-O3
	-fno-stack-clash-protection
	-fno-ident
	-fstrub=disable
	-fno-harden-compares
	-fno-harden-conditional-branches
	-fno-harden-control-flow-redundancy
	-fno-hardcfr-check-exceptions
	-fno-hardcfr-check-returning-calls
	-fhardcfr-check-noreturn-calls=never
	-fhardcfr-skip-leaf
	-fno-stack-check
	-ffunction-sections
	-fdata-sections
	-fzero-call-used-regs=skip
	-fzero-init-padding-bits=standard
	-fdelete-null-pointer-checks
	-fstrict-aliasing
	-fomit-frame-pointer
	-fno-rtti
	-ffp-contract=fast)
check_linker_option(OPTIONS_LINKER_RELEASE
	-Wl,-O3
	-Wl,--gc-sections
	-Wl,--no-copy-dt-needed-entries
	-Wl,--relax
	-Wl,--build-id=none)
add_link_options("$<$<CONFIG:Release>:${OPTIONS_LINKER_RELEASE}>")
target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:${OPTIONS_RELEASE}>")
if(VITA)
    if(NOT IS_DIRECTORY "${SUNSHINE_ASSETS_PATH}")
        message(FATAL_ERROR "SUNSHINE_ASSETS_PATH does not exist: ${SUNSHINE_ASSETS_PATH}")
    endif()
    set(SUNSHINE_ASSET_FILES)
    file(GLOB_RECURSE SUNSHINE_ASSET_FILES RELATIVE "${SUNSHINE_ASSETS_PATH}" "${SUNSHINE_ASSETS_PATH}/*")
    set(VPK_FILES)
    foreach(RELATIVE_FILE IN LISTS SUNSHINE_ASSET_FILES)
        set(SOURCE_FILE "${SUNSHINE_ASSETS_PATH}/${RELATIVE_FILE}")
        if(NOT IS_DIRECTORY "${SOURCE_FILE}")
            list(APPEND VPK_FILES FILE "${SOURCE_FILE}" "${RELATIVE_FILE}")
            message(STATUS "Asset: ${RELATIVE_FILE}")
        endif()
    endforeach()

    vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME} STRIPPED NOASLR)
    vita_create_vpk(${PROJECT_NAME}.vpk "OSS054231" ${PROJECT_NAME}.self VERSION "00.00" NAME ${PROJECT_NAME}
        FILE "${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/icon0.png" "sce_sys/icon0.png"
        FILE "${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/bg.png" "sce_sys/livearea/contents/bg.png"
        FILE "${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/startup.png" "sce_sys/livearea/contents/startup.png"
        FILE "${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/template.xml" "sce_sys/livearea/contents/template.xml"
        ${VPK_FILES}
    )
endif()
