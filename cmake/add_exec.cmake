if(ANDROID)
	if(TERMUX)
		set(APP_TARGET ${PROJECT_NAME})
		add_executable(${APP_TARGET}
                	${MAIN_HEADERS}
                	${MAIN_SOURCE}
                	${BINDING_HEADERS}
                	${BINDING_SOURCE}
                	${EMBEDDED_SOURCE})
	else()
		set(APP_TARGET main)
		add_library(${APP_TARGET} SHARED
                	${MAIN_HEADERS}
                	${MAIN_SOURCE}
                	${BINDING_HEADERS}
                	${BINDING_SOURCE}
                	${EMBEDDED_SOURCE})
	endif()
elseif(VITA)
	set(APP_TARGET ${PROJECT_NAME})
	add_executable(${PROJECT_NAME}
                	${MAIN_HEADERS}
                	${MAIN_SOURCE}
                	${BINDING_HEADERS}
                	${BINDING_SOURCE}
                	${EMBEDDED_SOURCE})	
else()
	set(APP_TARGET ${PROJECT_NAME})
	add_executable(${APP_TARGET} MACOSX_BUNDLE WIN32
		${MAIN_HEADERS}
		${MAIN_SOURCE}
		${BINDING_HEADERS}
		${BINDING_SOURCE}
		${EMBEDDED_SOURCE})
endif()
