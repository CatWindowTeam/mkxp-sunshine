cmake_minimum_required(VERSION 3.20)
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are disabled. Use: cmake -S . -B build")
endif()
project(_______ C)
include(FindPackageHandleStandardArgs)
include(CheckCCompilerFlag)
include(CheckLinkerFlag)
if(VITA)
    if(NOT DEFINED VITASDK_PATH OR VITASDK_PATH STREQUAL "")
        if(DEFINED ENV{VITASDK})
            set(VITASDK_PATH "$ENV{VITASDK}")
        else()
            set(VITASDK_PATH "/usr/local/vitasdk")
        endif()
    endif()
    include("${VITASDK_PATH}/share/vita.cmake" REQUIRED)
    set(VITA_APP_NAME ${PROJECT_NAME})
    set(VITA_TITLEID "OSS054231")
    set(VITA_VERSION "00.00")
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

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug
    Release
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(EMBEDDED_INPUT
	assets/icon.png
	assets/default.png
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
	list(APPEND ${outvar} ${outputfile})
endmacro()

foreach(item ${EMBEDDED_INPUT})
	ProcessWithXXD(EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

check_option(OPTIONS_DEBUG
	-g
	-Og)

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
	-ffunction-sections
	-fdata-sections
	-ftrivial-auto-var-init=uninitialized
	-fzero-call-used-regs=skip
	-fomit-frame-pointer)

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

add_executable(${PROJECT_NAME} ${EMBEDDED_SOURCE} main.c)
add_link_options("$<$<CONFIG:Release>:${OPTIONS_LINKER_RELEASE}>")
target_compile_options(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:${OPTIONS_DEBUG}>"
    "$<$<CONFIG:Release>:${OPTIONS_RELEASE}>")
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_net CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFINES})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_net::SDL3_net)
