include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/xxd.cmake)

# recursive search instead of hardcored list
file(GLOB_RECURSE MAIN_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h*")
file(GLOB_RECURSE MAIN_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*")
file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${BINDING_PATH}/*.h*")
file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${BINDING_PATH}/*.c*")
set(EMBEDDED_INPUT
	shader/common.h
	shader/transSimple.frag
	shader/trans.frag
	shader/hue.frag
	shader/sprite.frag
	shader/worldMachine.frag
	shader/water.frag
	shader/crt.frag
	shader/tilemapWater.frag
	shader/plane.frag
	shader/gray.frag
	shader/bitmapBlit.frag
	shader/flatColor.frag
	shader/simple.frag
	shader/simpleColor.frag
	shader/simpleAlpha.frag
	shader/simpleAlphaUni.frag
	shader/dynamicLight.frag
	shader/flashMap.frag
	shader/obscured.frag
	shader/minimal.vert
	shader/simple.vert
	shader/simpleColor.vert
	shader/sprite.vert
	shader/tilemap.vert
	shader/blur.frag
	shader/blurH.vert
	shader/blurV.vert
	shader/simpleMatrix.vert
	assets/icon.png
	assets/gamecontrollerdb.txt
	assets/crash.png
	assets/noise.png
	assets/the_modded_machine.png
	binding-mri/module_rpg1.rb
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND MAIN_SOURCE assets/resources.rc)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND MAIN_HEADERS src/mac-desktop.h)
    list(APPEND MAIN_SOURCE src/mac-desktop.mm)
endif()

if(STEAM)
	list(APPEND MAIN_HEADERS steamshim/steamshim_child.h)
	list(APPEND MAIN_SOURCE steamshim/steamshim_child.c)
endif()

foreach(item ${EMBEDDED_INPUT})
	ProcessWithXXD(EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

find_program(GLSLANG_EXE NAMES glslangValidator glslang)
if(GLSLANG_EXE)
	foreach(item uber.vert uber.frag light.frag)
		set(gpu_spv ${CMAKE_CURRENT_BINARY_DIR}/${item}.spv)
		set(gpu_hdr ${CMAKE_CURRENT_BINARY_DIR}/${item}.spv.xxd)
		add_custom_command(
			OUTPUT ${gpu_spv}
			COMMAND ${GLSLANG_EXE} -V ${CMAKE_CURRENT_SOURCE_DIR}/shader/gpu/${item} -o ${gpu_spv}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shader/gpu/${item}
			COMMENT "Compiling GPU shader ${item} to SPIR-V"
		)
		add_custom_command(
			OUTPUT ${gpu_hdr}
			COMMAND ${XXD_EXE} -i ${item}.spv ${gpu_hdr}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS ${gpu_spv}
			COMMENT "Generating XXD for ${item}.spv"
		)
		set_source_files_properties(${gpu_hdr} PROPERTIES HEADER_FILE_ONLY TRUE)
		list(APPEND EMBEDDED_SOURCE ${gpu_hdr})
	endforeach()
	list(APPEND DEFINES SUNSHINE_GPU_SHADERS)
else()
	message(STATUS "glslangValidator not found, GPU render backend disabled")
endif()
