include(FindPackageHandleStandardArgs)
find_package(PkgConfig REQUIRED)
include(${CMAKE_SOURCE_DIR}/cmake/deps_config.cmake)
add_subdirectory(${CMAKE_SOURCE_DIR}/external)

# TODO: make as submodule too later
pkg_check_modules(SIGCPP REQUIRED sigc++-3.0)

target_compile_definitions(${APP_TARGET}
							PRIVATE
							${DEFINES})
							
target_include_directories(${APP_TARGET} 
							PRIVATE
							src
							external/robin-map/include
							${SIGCPP_INCLUDE_DIRS}
							${BINDING_INCLUDE})

target_link_libraries(${APP_TARGET}
						PRIVATE
						SDL3_image::SDL3_image
						SDL3_ttf::SDL3_ttf
						SDL3_net::SDL3_net
						SDL3_mixer::SDL3_mixer
						SDL3::SDL3
						${SIGCPP_LIBRARIES}
						${PLATFORM_LIBRARIES}
						${BINDING_LIBRARY}
						CLI11::CLI11
						ZLIB::ZLIBSTATIC
						PhysFS::PhysFS)
