include(FindPackageHandleStandardArgs)
find_package(PkgConfig REQUIRED)
find_package(tsl-robin-map REQUIRED)
find_package(ZLIB REQUIRED)
	find_package(Ruby 3.4
		COMPONENTS Interpreter Development
		REQUIRED)
if(NOT TARGET SDL3::SDL3)
	find_package(SDL3 CONFIG REQUIRED)
endif()
if(NOT TARGET SDL3_image::SDL3_image)
	find_package(SDL3_image CONFIG REQUIRED)
endif()
if(NOT TARGET SDL3_mixer::SDL3_mixer)
	find_package(SDL3_mixer CONFIG REQUIRED)
endif()
if(NOT TARGET SDL3_ttf::SDL3_ttf)
	find_package(SDL3_ttf CONFIG REQUIRED)
endif()

# SDL_net doesn't support PSP so lets disable it
if(NOT PSP)
	if(NOT TARGET SDL3_net::SDL3_net)
		find_package(SDL3_net REQUIRED)
	endif()
	target_compile_definitions(${APP_TARGET} PRIVATE JOURNAL_ENABLED=1)
else()
	target_compile_definitions(${APP_TARGET} PRIVATE JOURNAL_ENABLED=0)
endif()

find_package(PhysFS REQUIRED)
find_path(PIXMAN_INCLUDE_DIR NAMES pixman.h PATH_SUFFIXES pixman-1)
#wtf is this?
find_library(PIXMAN_LIBRARY NAMES pixman-1 pixman-1_static pixman-1_staticd)
find_package_handle_standard_args(pixman-1 DEFAULT_MSG PIXMAN_LIBRARY PIXMAN_INCLUDE_DIR)
mark_as_advanced(PIXMAN_INCLUDE_DIR PIXMAN_LIBRARY)
pkg_check_modules(SIGC2 REQUIRED sigc++-2.0)
# ради баланса вселенной
if(ANDROID AND NOT TERMUX)
	find_library(SIGC2_SHARED_LIBRARY NAMES sigc-2.0 PATHS ${SIGC2_LIBRARY_DIRS} NO_DEFAULT_PATH)
	if(SIGC2_SHARED_LIBRARY)
		file(COPY ${SIGC2_SHARED_LIBRARY} DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/android-project/app/libs/${ANDROID_ABI}")
	else()
		message(WARNING "Could not locate the sigc++-2.0 shared library to bundle into the APK")
	endif()
endif()

target_compile_definitions(${APP_TARGET} PRIVATE ${DEFINES})
target_include_directories(${APP_TARGET} PRIVATE
	src
	include
	${SDL_INCLUDE_DIRS}
	${PIXMAN_INCLUDE_DIR}
	${SIGC2_INCLUDE_DIRS}
	${Ruby_INCLUDE_DIRS}
	${PHYSFS_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIRS}
	SDL3::Headers
	SDL3_mixer::SDL3_mixer
	SDL3_net::SDL3_net)
target_link_directories(${APP_TARGET} PRIVATE ${SIGC2_LIBRARY_DIRS})

target_link_libraries(${APP_TARGET} PRIVATE
	SDL3_image::SDL3_image
	SDL3_mixer::SDL3_mixer
	SDL3_ttf::SDL3_ttf
	SDL3_net::SDL3_net
	SDL3::SDL3
	${PHYSFS_LIBRARY}
	${PIXMAN_LIBRARY}
	${SIGC2_LIBRARIES}
	${PLATFORM_LIBRARIES}
	${Ruby_LIBRARIES}
	ZLIB::ZLIB
	physfs
	tsl::robin_map)
