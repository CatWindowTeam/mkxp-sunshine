if(BINDING STREQUAL "mruby")
	add_library(mruby STATIC "${CMAKE_CURRENT_SOURCE_DIR}/external/mruby/mruby.c")
	set(BINDING_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/external/mruby")
	set(BINDING_LIBRARY mruby)
	
	file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/mruby/*.h*")
	file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/mruby/*.c*")
	find_program(MRBC_EXE NAMES mrbc mrbc.exe REQUIRED)

	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/module_rpg.mrb"
		COMMAND ${MRBC_EXE} "-o${CMAKE_CURRENT_BINARY_DIR}/module_rpg.mrb" "${CMAKE_CURRENT_SOURCE_DIR}/bindings/mruby/module_rpg.rb"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/mruby/module_rpg.rb"
		COMMENT "Precompiling module_rpg.rb"
	)
	
	ProcessWithXXDIn(${CMAKE_CURRENT_BINARY_DIR} EMBEDDED_SOURCE "module_rpg.mrb" ".")
elseif(BINDING STREQUAL "cruby")
	find_package(Ruby 3.3 COMPONENTS Interpreter Development REQUIRED)
    set(BINDING_LIBRARY ${Ruby_LIBRARIES})
    set(BINDING_INCLUDE ${Ruby_INCLUDE_DIRS})
    file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/mri/*.h*")
    file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/mri/*.c*")
    add_definitions(-DRUBY_DONT_SUBST)
	
	ProcessWithXXD(EMBEDDED_SOURCE bindings/mri/module_rpg1.rb ${CMAKE_CURRENT_BINARY_DIR})
elseif(BINDING STREQUAL "benchmark")
	file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/benchmark/*.h*")
	file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings/benchmark/*.c*")
else()
	message(FATAL_ERROR "unknown binding selected")
endif()
