cmake_minimum_required(VERSION 3.20)
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(_______ C)
include(CheckIPOSupported)
include(${CMAKE_SOURCE_DIR}/cmake/check.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/xxd.cmake)
include(${CMAKE_SOURCE_DIR}/deps_config.cmake)
set(EMBEDDED_INPUT assets/icon.png assets/default.png)

# LTO shit
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error LANGUAGES C)
if(NOT MinGW)
	if(ipo_supported)
	   	set_property(TARGET ${APP_TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	   	check_option_c(OPTIONS_LTO -fdevirtualize-at-ltrans)	
	else()
	   	message(WARNING "IPO/LTO is not supported: ${ipo_error}")
	endif()
else()
	message(WARNING "IPO/LTO is not supported on MinGW/MSYS2, TODO: fix it")
endif()

# prepare embeds
foreach(item ${EMBEDDED_INPUT})
	ProcessWithXXD(EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()


add_executable(${PROJECT_NAME} ${EMBEDDED_SOURCE} main.c)
check_linker_option_c(LOPT_BASE
	LINKER:-O3
	LINKER:--gc-sections
	LINKER:--no-copy-dt-needed-entries
	LINKER:--icf=safe
	LINKER:--strip-all
	LINKER:--relax
	LINKER:--build-id=none
	LINKER:--as-needed
	LINKER:--no-whole-archive
	LINKER:-z,start-stop-gc
	LINKER:--no-eh-frame-hdr
)

target_link_options(${PROJECT_NAME} PRIVATE ${LOPT_BASE}) 
set(EXTERNAL_DIR "${CMAKE_CURRENT_LIST_DIR}/../external")
set(EXTERNAL_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/external")
add_subdirectory("${EXTERNAL_DIR}/SDL" "${EXTERNAL_BUILD_DIR}/SDL" EXCLUDE_FROM_ALL)
add_subdirectory("${EXTERNAL_DIR}/SDL_image" "${EXTERNAL_BUILD_DIR}/SDL_image" EXCLUDE_FROM_ALL)
add_subdirectory("${EXTERNAL_DIR}/SDL_net" "${EXTERNAL_BUILD_DIR}/SDL_net" EXCLUDE_FROM_ALL)
check_option_c(OPTIONS_RELEASE
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
	-fmerge-all-constants
	-fgraphite
	-floop-block
	-fgraphite-identity
	-floop-nest-optimize
	-floop-parallelize-all
	-floop-interchange
)

set(targets SDL3_net-static SDL3_image-static SDL3-static ${PROJECT_NAME})
foreach(t IN LISTS targets)
    if(TARGET "${t}")
        message(STATUS "Configuring ${t}")
        target_compile_options(${t} PRIVATE ${OPTIONS_RELEASE})
		if(ipo_supported)
			set_property(TARGET ${t} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
			target_compile_options(${t} PRIVATE ${OPTIONS_RELEASE} ${OPTIONS_LTO})
		endif()
    else()
        message(WARNING "${t} not found, skip")
    endif()
endforeach()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image SDL3_net::SDL3_net SDL3::SDL3)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_options(${PROJECT_NAME} PRIVATE -mwindows -municode)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE)
endif()
