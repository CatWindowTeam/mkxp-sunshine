include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/check.cmake)

#load profile
if(NOT "${CPU_OPT_PROFILE}" STREQUAL "")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/arch/${CPU_OPT_PROFILE}.cmake)
endif()

# Common arguments on specific CPU arch
if("${ARCH_COMMON}" STREQUAL "")
	message(STATUS "ARCH_COMMON empty")
else()
	check_option(ARCH_COMMON_OPTIONS ${ARCH_COMMON})
	add_compile_options(${ARCH_COMMON_OPTIONS})
endif()

# Loading release options specific for cpu arch from toolchain file
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	if("${ARCH_RELEASE}" STREQUAL "")
		message(STATUS "ARCH_RELEASE empty")
	else()
		check_option(ARCH_RELEASE_OPTIONS ${ARCH_RELEASE})
	endif()
endif()

# Load platfrom specific debug options for compiler from toolchain file
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	if("${ARCH_DEBUG}" STREQUAL "")
		message(STATUS "ARCH_DEBUG empty")
	else()
		check_option(ARCH_DEBUG_OPTIONS ${ARCH_DEBUG})
	endif()
endif()
