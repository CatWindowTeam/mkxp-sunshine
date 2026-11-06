if(BINDING STREQUAL "mruby")
	if(MRUBY_AUTOBUILD)
		set(_mruby_flags
			${COPT_C}
			${COPT_CXX}
			${COPT_UNSAFE}
			${COPT_UNSAFE_CXX}
			${COPT_NATIVE}
			${COPT_LTO}
		)
		
		string(REPLACE ";" " " _mruby_flags "${_mruby_flags}")
		string(STRIP "${_mruby_flags}" _mruby_flags)
		execute_process(COMMAND "${CMAKE_COMMAND}" -E env
		        "MRUBY_BUILD_FLAGS=${_mruby_flags}"
		        "LOW_MEMORY=${LOW_MEMORY}"
		        "PLATFORM=${CMAKE_SYSTEM_NAME}"
		        "${CMAKE_SOURCE_DIR}/cmake/mruby.sh"
		        "${CMAKE_SOURCE_DIR}"
		)
	endif()
    file(GLOB_RECURSE BINDING_LIBRARY LIST_DIRECTORIES FALSE "${CMAKE_CURRENT_SOURCE_DIR}/external/mruby/build/sunshine/lib/libmruby.a")

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/mruby/build/sunshine/include")
    	set(BINDING_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/external/mruby/build/sunshine/include")
    else()
    	message(FATAL_ERROR "Failed to find MRuby includes")
    endif()

	file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/binding-${BINDING}/*.h*")
	file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/binding-${BINDING}/*.c*")

	set(MRBC_EXE "${CMAKE_CURRENT_SOURCE_DIR}/external/mruby/build/host/bin/mrbc")

	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/module_rpg.mrb"
		COMMAND ${MRBC_EXE} "-o${CMAKE_CURRENT_BINARY_DIR}/module_rpg.mrb" "./bindings/binding-${BINDING}/module_rpg.rb"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DEPENDS "./bindings/binding-${BINDING}/module_rpg.rb"
		COMMENT "Precompiling module_rpg.rb"
	)

	ProcessWithXXDIn(${CMAKE_CURRENT_BINARY_DIR} EMBEDDED_SOURCE "module_rpg.mrb" ".")
elseif(BINDING STREQUAL "cruby")
	find_package(Ruby 3.3 COMPONENTS Interpreter Development REQUIRED)
    set(BINDING_LIBRARY ${Ruby_LIBRARIES})
    set(BINDING_INCLUDE ${Ruby_INCLUDE_DIRS})
    file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/binding-mri/*.h*")
    file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/binding-mri/*.c*")
    add_definitions(-DRUBY_DONT_SUBST)
	
	ProcessWithXXD(EMBEDDED_SOURCE bindings/binding-mri/module_rpg1.rb ${CMAKE_CURRENT_BINARY_DIR})
elseif(BINDING STREQUAL "benchmark")
	file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/binding-benchmark/*.h*")
	file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/binding-benchmark/*.c*")
else()
	message(FATAL_ERROR "unknown binding selected")
endif()
