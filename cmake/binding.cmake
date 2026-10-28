if(BINDING STREQUAL "mruby")
	if(MRUBY_AUTOBUILD)
		if(UNSAFE_OPTIMIZATIONS)
			check_option(OPTIONS_RELEASE_UNSAFE
				-fstrub=disable
		        -fvtable-verify=none
		        -fno-enforce-eh-specs
		        -fno-hardcfr-check-exceptions
	            -fno-hardcfr-check-returning-calls
		        -fhardcfr-check-noreturn-calls=never
				-fstack-check=no
				-mno-speculative-load-hardening
				-fzero-call-used-regs=skip
				-fno-threadsafe-statics
				-mno-lvi-hardening)
		endif()

		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			check_option(OPTIONS_DEBUG -g -Og)
		elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
			if(LOW_MEMORY)
				check_option(COMPILER_OPTIONS
					-Oz
					-ffunction-sections
					-fdata-sections
					-fno-rtti
					-ffp-contract=fast
					-fno-unwind-tables
					-fno-asynchronous-unwind-tables
					-fdelete-dead-exceptions
					-fwhole-program-vtables
					-fvirtual-function-elimination
					-fpack-struct=1
					-fshort-enums
					-fshort-wchar
					-fshort-double
					-fconserve-stack)
			else()
				check_option(COMPILER_OPTIONS
					-O3
					-ffunction-sections
					-fdata-sections
					-fno-rtti
					-ffp-contract=fast
					-fno-unwind-tables
					-fno-asynchronous-unwind-tables
					-fdelete-dead-exceptions
					-fwhole-program-vtables
					-fvirtual-function-elimination)
			endif()
		endif()
		set(_mruby_flags "${COMPILER_OPTIONS} ${OPTIONS_RELEASE_UNSAFE} ${OPTIONS_DEBUG}")
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
    file(GLOB_RECURSE BINDING_LIBRARY LIST_DIRECTORIES FALSE "${CMAKE_CURRENT_SOURCE_DIR}/external/mruby/build/host/lib/libmruby.a")

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/mruby/build/host/include")
    	set(BINDING_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/external/mruby/build/host/include")
    else()
    	message(FATAL_ERROR "Failed to find MRuby includes")
    endif()

    file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/binding-${BINDING}/*.h*")
    file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/binding-${BINDING}/*.c*")
elseif(BINDING STREQUAL "cruby")
	find_package(Ruby 3.3 COMPONENTS Interpreter Development REQUIRED)
    set(BINDING_LIBRARY ${Ruby_LIBRARIES})
    set(BINDING_INCLUDE ${Ruby_INCLUDE_DIRS})
    file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/binding-mri/*.h*")
    file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/binding-mri/*.c*")
    add_definitions(-DRUBY_DONT_SUBST)
elseif(BINDING STREQUAL "benchmark")
	file(GLOB_RECURSE BINDING_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/binding-benchmark/*.h*")
	file(GLOB_RECURSE BINDING_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/binding-benchmark/*.c*")
else()
	message(FATAL_ERROR "unknown binding selected")
endif()
