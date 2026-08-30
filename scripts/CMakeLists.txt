file(GLOB_RECURSE scripts CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/scripts/*.rb")

find_program(RUBY_EXE ruby DOC "Location of the Ruby executable")
set(game_dir "${CMAKE_BINARY_DIR}/bin")
set(data_dir "${game_dir}/Data")
set(xscripts "${data_dir}/xScripts.rxdata")
add_custom_target(data_dir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${data_dir}")
add_custom_command(
  OUTPUT "${xscripts}"
  COMMAND "${RUBY_EXE}" "${CMAKE_SOURCE_DIR}/rpgscript.rb" "${CMAKE_CURRENT_LIST_DIR}" "${game_dir}"
  DEPENDS ${scripts} data_dir "${CMAKE_SOURCE_DIR}/rpgscript.rb"
  COMMENT "Bundling xScripts.rxdata")
add_custom_target(xScripts ALL DEPENDS "${xscripts}")
