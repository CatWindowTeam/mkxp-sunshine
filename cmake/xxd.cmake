find_program(XXD_EXE
  NAMES xxd tinyxxd xxd.exe
  REQUIRED
  HINTS
    ENV PATH
    /usr/bin /usr/local/bin .
)

macro(ProcessWithXXDIn workdir outvar inputfile outdir)
	get_filename_component(basefile ${inputfile} NAME)
	set(outputfile ${outdir}/${basefile}.xxd)
	#set_source_files_properties(${outputfile} PROPERTIES HEADER_FILE_ONLY TRUE)
	add_custom_command(
		OUTPUT ${outputfile}
		COMMAND ${XXD_EXE} -i ${inputfile} ${outputfile}
		WORKING_DIRECTORY ${workdir}
		DEPENDS ${inputfile}
		COMMENT "Generating XXD for ${inputfile}"
	)
	list(APPEND ${outvar} ${outputfile})
endmacro()

macro(ProcessWithXXD outvar inputfile outdir)
	ProcessWithXXDIn(${CMAKE_CURRENT_SOURCE_DIR} ${outvar} ${inputfile} ${outdir})
endmacro()