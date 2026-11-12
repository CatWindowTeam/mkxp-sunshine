if(BINDING STREQUAL "mruby")
	add_library(mruby STATIC "${CMAKE_CURRENT_SOURCE_DIR}/external/mruby/mruby.c")
	set(BINDING_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/external/mruby")
	set(BINDING_LIBRARY mruby)
	
	file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/binding-${BINDING}/*.h*")
	file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/binding-${BINDING}/*.c*")
	find_program(MRBC_EXE NAMES mrbc mrbc.exe REQUIRED)

	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/module_rpg.mrb"
		COMMAND ${MRBC_EXE} "-o${CMAKE_CURRENT_BINARY_DIR}/module_rpg.mrb" "${CMAKE_CURRENT_SOURCE_DIR}/bindings/binding-mruby/module_rpg.rb"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/binding-mruby/module_rpg.rb"
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
