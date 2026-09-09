cmake_minimum_required(VERSION 3.28.3)
project(_______ C)
include(FindPackageHandleStandardArgs)
option(DEBUG "Debug mode" ON)
option(SANITIZERS "Enable sanitizers" OFF)
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)
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

add_executable(${PROJECT_NAME} ${EMBEDDED_SOURCE} main.c)
add_link_options(-Wl,-O3 -Wl,--gc-sections -Wl,-z,noexecstack)
target_compile_options(${PROJECT_NAME} PRIVATE -O3)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_net CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFINES})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_net::SDL3_net)
