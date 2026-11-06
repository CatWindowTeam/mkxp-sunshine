include(FindPackageHandleStandardArgs)
find_package(PkgConfig REQUIRED)
include(${CMAKE_SOURCE_DIR}/cmake/deps_config.cmake)
add_subdirectory(${CMAKE_SOURCE_DIR}/external)

target_include_directories(${APP_TARGET} 
							PRIVATE
							${PLATFORM_INCLUDE}
							${CMAKE_SOURCE_DIR}/src
							${CMAKE_SOURCE_DIR}/external/robin-map/include
							${CMAKE_SOURCE_DIR}/external/sigc
							${CMAKE_BINARY_DIR}/external/sigc
							${BINDING_INCLUDE}
							${CMAKE_SOURCE_DIR}
							${CMAKE_BINARY_DIR})

target_link_libraries(${APP_TARGET}
						PRIVATE
						SDL3_image::SDL3_image
						SDL3_ttf::SDL3_ttf
						SDL3_net::SDL3_net
						SDL3_mixer::SDL3_mixer
						SDL3::SDL3
						${PLATFORM_LIBRARIES}
						${BINDING_LIBRARY}
						CLI11::CLI11
						ZLIB::ZLIBSTATIC
						PhysFS::PhysFS
						sigc-3.0)
