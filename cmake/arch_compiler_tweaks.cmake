include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/check.cmake)
# Common arguments on specific CPU arch
if("${ARCH_COMMON}" STREQUAL "")
	message(STATUS "ARCH_COMMON empty")
else()
	check_option(ARCH_COMMON_OPTIONS ${ARCH_COMMON})
	add_compile_options(${ARCH_COMMON_OPTIONS})
endif()

# Loading release options specific for cpu arch from toolchain file
if("${ARCH_RELEASE}" STREQUAL "")
	message(STATUS "ARCH_RELEASE empty")
else()
	check_option(ARCH_RELEASE_OPTIONS ${ARCH_RELEASE})
endif()

# Load platfrom specific debug options for compiler from toolchain file
if("${ARCH_DEBUG}" STREQUAL "")
	message(STATUS "ARCH_DEBUG empty")
else()
	check_option(ARCH_DEBUG_OPTIONS ${ARCH_DEBUG})
endif()

# Load platfrom specific release options for linker from toolchain file
if("${ARCH_LINKER_RELEASE}" STREQUAL "")
	message(STATUS "ARCH_LINKER_RELEASE empty")
else()
	check_option(ARCH_RELEASE_OPTIONS_LINKER ${ARCH_LINKER_RELEASE})
endif()
add_link_options("$<$<CONFIG:Release>:${ARCH_RELEASE_OPTIONS_LINKER}>")
