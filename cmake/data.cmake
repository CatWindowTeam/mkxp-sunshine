# shared compiler and linker options
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
		set(COPT_BASE_WIN_UC
			$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fno-unwind-tables>
			$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fno-asynchronous-unwind-tables>
		)
	endif()

	if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	  check_option(COPT_C
	    /O2
	    /GF
	    /fp:fast
	    /Gy
	    /Gw
	  )
	else()
	  check_option(COPT_C
	    -O3
	    -fno-ident
	    -ffunction-sections
	    -fdata-sections
	    -ffp-contract=fast
	    -fdelete-dead-exceptions
	    -fmerge-all-constants
	    # Graphite
		-fgraphite
	    -floop-block
	    -fgraphite-identity
	    -floop-nest-optimize
	    -floop-parallelize-all
	    -floop-interchange
	  )
	endif()

	if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	  check_option(COPT_CXX GR-)
	else()
	  check_option(COPT_CXX
	    -fno-rtti
	    -fwhole-program-vtables
	    -fvirtual-function-elimination
	  )
	endif()

	if(MSVC)
	  check_linker_option(LOPT_BASE
	    /OPT:ICF
	    /OPT:LBR
	    /OPT:REF
	    /NOLOGO
	  )
	else()
	  check_linker_option(LOPT_BASE
		LINKER:-O3
		LINKER:--gc-sections
		LINKER:--no-copy-dt-needed-entries
		LINKER:--icf=safe
		LINKER:--strip-all
		LINKER:--relax
		LINKER:--build-id=none
		LINKER:--as-needed
		LINKER:--no-whole-archive
		LINKER:-z,start-stop-gc
		LINKER:--no-eh-frame-hdr
	  )
	endif()

	if(UNSAFE_OPTIMIZATIONS)
		if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		  check_option(COPT_UNSAFE
		  	/Zc:tlsGuards
		  	/Zc:threadSafeInit
		  	/GS
		  )
		else()
		  check_option(COPT_UNSAFE
		  	-fstrub=disable
		  	-fno-hardcfr-check-exceptions
		  	-fno-hardcfr-check-returning-calls
		  	-fhardcfr-check-noreturn-calls=never
		  	-fstack-check=no
		  )
		endif()

		if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		  check_option(COPT_UNSAFE_CXX
		  	/Zc:tlsGuards-
		  	/Zc:threadSafeInit-
		  	/GS-
		  )
		else()
		  check_option(COPT_UNSAFE_CXX
		  	-fvtable-verify=none
		  	-fno-enforce-eh-specs
		  	-fno-threadsafe-statics
		  )
		endif()
	endif()
else()
	if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	  check_option(COPT_C
	    /DEBUG:FULL
	  )
	else()
	  check_option(COPT_C
	    -g
	    -Og
	  )
	endif()
endif()

if(NATIVE)
	if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	  #a
	else()
	  check_option(COPT_NATIVE
	    -march=native
	    -mtune=native
	  )
	endif()
	add_compile_options(${COPT_NATIVE})
endif()

if(LTO)
	if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	  #a
	else()
	  check_option(COPT_LTO
	    -fdevirtualize-at-ltrans
	    -fwhole-program-vtables
	  )
	endif()
endif()
