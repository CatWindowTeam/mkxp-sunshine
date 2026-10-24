include(FindPackageHandleStandardArgs)
find_package(PkgConfig REQUIRED)
include(${CMAKE_SOURCE_DIR}/cmake/deps_config.cmake)
add_subdirectory(${CMAKE_SOURCE_DIR}/external)

# shit that doesn't support CMake
find_path(PIXMAN_INCLUDE_DIR NAMES pixman.h PATH_SUFFIXES pixman-1)
find_library(PIXMAN_LIBRARY NAMES pixman-1 pixman-1_static pixman-1_staticd)
find_package_handle_standard_args(pixman-1 DEFAULT_MSG PIXMAN_LIBRARY PIXMAN_INCLUDE_DIR)
mark_as_advanced(PIXMAN_INCLUDE_DIR PIXMAN_LIBRARY)

# TODO: make as submodule too later
pkg_check_modules(SIGC2 REQUIRED sigc++-2.0)

target_compile_definitions(${APP_TARGET}
							PRIVATE
							${DEFINES})
							
target_include_directories(${APP_TARGET} 
							PRIVATE
							src
							external/robin-map/include
							${SIGC2_INCLUDE_DIRS}
							${PIXMAN_INCLUDE_DIR}
							${BINDING_INCLUDE})

target_link_libraries(${APP_TARGET}
						PRIVATE
						SDL3_image::SDL3_image
						SDL3_ttf::SDL3_ttf
						SDL3_net::SDL3_net
						SDL3_mixer::SDL3_mixer
						SDL3::SDL3
						${PIXMAN_LIBRARY}
						${SIGC2_LIBRARIES}
						${PLATFORM_LIBRARIES}
						${BINDING_LIBRARY}
						CLI11::CLI11
						ZLIB::ZLIBSTATIC
						PhysFS::PhysFS)
