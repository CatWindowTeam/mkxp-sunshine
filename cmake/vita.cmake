#prepare Vita specific thinks
function(vita_prepare)
	if(VITA)
    	if(NOT DEFINED VITASDK_PATH OR VITASDK_PATH STREQUAL "")
        	if(DEFINED ENV{VITASDK})
            	set(VITASDK_PATH "$ENV{VITASDK}")
        	else()
            	set(VITASDK_PATH "/usr/local/vitasdk")
        	endif()
    	endif()
    	include("${VITASDK_PATH}/share/vita.cmake" REQUIRED)

    	set(VITA_APP_NAME "Oneshot: Sunshine")
    	set(VITA_TITLEID "OSS936174")
    	set(VITA_VERSION "00.13")

    	# SDL3_mixer CMake helper modules
    	list(PREPEND CMAKE_MODULE_PATH "${VITASDK_PATH}/SDL_mixer/cmake")
    	if(NOT EXISTS "${VITASDK_PATH}/SDL_mixer/cmake/PkgConfigHelper.cmake")
        	message(FATAL_ERROR "PkgConfigHelper.cmake not found in ${VITASDK_PATH}/SDL_mixer/cmake")
    	endif()
	endif()
endfunction()

function(vita_finish)
	if(VITA)
		add_custom_command(
			OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshine.zip
		    COMMENT "Creating sunshine.zip ..."
		    COMMAND mkdir ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshune_zip
		    COMMAND mkdir ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshune_zip/Data
		    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SUNSHINE_ASSETS_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/build/sunshune_zip/"
		    COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/rpgscript.rb ${CMAKE_CURRENT_SOURCE_DIR}/scripts/ ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshune_zip/
		    COMMAND zip -X -9 -r ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshine.zip ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshune_zip/ -x ".git/*" "*/.git/*"
			DEPENDS "${SUNSHINE_ASSETS_PATH}"
		    VERBATIM
		)
		vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME} STRIPPED NOASLR)
		vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
		  VERSION ${VITA_VERSION}
		  NAME ${VITA_APP_NAME}
		  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/icon0.png sce_sys/icon0.png
		  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
		  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
		  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
		  FILE ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshine.zip sunshine.zip
		)
	endif()
endfunction()
