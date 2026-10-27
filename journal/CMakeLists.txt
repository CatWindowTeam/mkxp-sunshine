cmake_minimum_required(VERSION 3.20)
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are disabled. Use: cmake -S . -B build")
endif()
project(_______ C CXX)
include(../cmake/check.cmake)
include(../cmake/xxd.cmake)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(EMBEDDED_INPUT assets/icon.png assets/default.png)
foreach(item ${EMBEDDED_INPUT})
	ProcessWithXXD(EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

add_executable(${PROJECT_NAME} ${EMBEDDED_SOURCE} main.c)
include(${CMAKE_SOURCE_DIR}/deps_config.cmake)

check_option(OPTIONS_RELEASE
			-O3
			-fno-ident
			-ffunction-sections
			-fdata-sections
			-fdelete-dead-exceptions
			-ffast-math
			-ffp-contract=fast
			-fstrub=disable
			-fno-hardcfr-check-exceptions
	        -fno-hardcfr-check-returning-calls
			-fhardcfr-check-noreturn-calls=never
			-fstack-check=no
			-fzero-call-used-regs=skip)
add_compile_options(${OPTIONS_RELEASE})
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_net EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image SDL3_net::SDL3_net SDL3::SDL3 ${PLATFORM_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFINES})
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_options(${PROJECT_NAME} PRIVATE -mwindows -municode)
	list(APPEND DEFINES UNICODE)
endif()
