# tweaks for some ports

function(ports_prepare)
	if(VITA)
    	if(NOT DEFINED VITASDK_PATH OR VITASDK_PATH STREQUAL "")
        	if(DEFINED ENV{VITASDK})
            	set(VITASDK_PATH "$ENV{VITASDK}")
        	else()
            	set(VITASDK_PATH "/usr/local/vitasdk")
        	endif()
    	endif()
    	include("${VITASDK_PATH}/share/vita.cmake" REQUIRED)
    	
    	# SDL3_mixer CMake helper modules
    	list(PREPEND CMAKE_MODULE_PATH "${VITASDK_PATH}/SDL_mixer/cmake")
    	if(NOT EXISTS "${VITASDK_PATH}/SDL_mixer/cmake/PkgConfigHelper.cmake")
        	message(FATAL_ERROR "PkgConfigHelper.cmake not found in ${VITASDK_PATH}/SDL_mixer/cmake")
    	endif()
    elseif(PS2)
		# adjust the default behavior of the FIND_XXX() commands:
	    # search programs in the host environment
	    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	
	    # search headers and libraries in the target environment
	    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)    	
	endif()
endfunction()

function(ports_finish)
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
		vita_create_vpk(${PROJECT_NAME}.vpk "OSS936174" ${PROJECT_NAME}.self
		  VERSION "00.13"
		  NAME "Oneshot: Sunshine"
		  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/icon0.png sce_sys/icon0.png
		  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
		  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
		  FILE ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
		  FILE ${CMAKE_CURRENT_SOURCE_DIR}/build/sunshine.zip sunshine.zip
		)
	elseif(PS2)
		set_target_properties(${APP_TARGET} PROPERTIES SUFFIX ".elf")
	endif()
endfunction()
