# shared compiler and linker options
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	check_option(COPT_C
	    -O3
	    -fno-ident
	    -pipe
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
	    ${EXTRA_COPT}
	)

	check_option(COPT_CXX
	    -fno-rtti
	)

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

	if(UNSAFE_OPTIMIZATIONS)
		check_option(COPT_UNSAFE
		  	-fstrub=disable
		  	-fno-hardcfr-check-exceptions
		  	-fno-hardcfr-check-returning-calls
		  	-fhardcfr-check-noreturn-calls=never
		  	-fstack-check=no
		)

		check_option(COPT_UNSAFE_CXX
		  	-fvtable-verify=none
		  	-fno-enforce-eh-specs
		  	-fno-threadsafe-statics
		)
	endif()
else()
	check_option(COPT_C
		-g
		-pipe
		-Og
	)
endif()

if(NATIVE)
	  check_option(COPT_NATIVE
	    -march=native
	    -mtune=native
	  )
	add_compile_options(${COPT_NATIVE})
endif()

if(LTO)
	  check_option(COPT_LTO
	    -fdevirtualize-at-ltrans
	    -fwhole-program-vtables
	    -fvirtual-function-elimination
	  )
endif()
