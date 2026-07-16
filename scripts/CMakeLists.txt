set(scripts
  Credits_Message.rb
  Data_FastTravel.rb
  Data_Footsteps.rb
  Data_Item.rb
  Data_Settings.rb
  Data_Settings_Classes.rb
  Data_Settings_Setters.rb
  Data_SpecialEventData.rb
  Desktop_Message.rb
  Doc_Message.rb
  EdText.rb
  Ed_Message.rb
  FastTravel.rb
  Game_Actor.rb
  Game_Actors.rb
  Game_BattleAction.rb
  Game_Battler_1.rb
  Game_Battler_2.rb
  Game_Battler_3.rb
  Game_Character_1.rb
  Game_Character_2.rb
  Game_Character_3.rb
  Game_CommonEvent.rb
  Game_Event.rb
  Game_FastTravel.rb
  Game_Follower.rb
  Game_Map.rb
  Game_Oneshot.rb
  Game_Party.rb
  Game_Picture.rb
  Game_Player.rb
  Game_Screen.rb
  Game_SelfSwitches.rb
  Game_Switches.rb
  Game_System.rb
  Game_Temp.rb
  Game_Variables.rb
  Game_Light.rb
  Interpreter_1.rb
  Interpreter_2.rb
  Interpreter_3.rb
  Interpreter_4.rb
  Interpreter_5.rb
  Interpreter_6.rb
  Interpreter_7.rb
  Main.rb
  Particles.rb
  Persistent.rb
  Puzzle_Film.rb
  Puzzle_Pixel.rb
  Puzzle_Safe.rb
  Puzzle_Sokoban.rb
  RPG.rb
  SaveLoad.rb
  Scene_Debug.rb
  Scene_Map.rb
  Scene_Name.rb
  Scene_Title.rb
  Script.rb
  Sprite_Character.rb
  Sprite_Footprint.rb
  Sprite_Footsplash.rb
  Sprite_Light.rb
  Sprite_MapText.rb
  Sprite_Picture.rb
  Sprite_Timer.rb
  Spriteset_Map.rb
  Window_Base.rb
  Window_DebugLeft.rb
  Window_DebugRight.rb
  Window_Help.rb
  Window_InputNumber.rb
  Window_Item.rb
  Window_MainMenu.rb
  Window_Message.rb
  Window_NameEdit.rb
  Window_NameInput.rb
  Window_Selectable.rb
  Window_Settings.rb
  i18n_English.rb
  i18n_Japanese.rb
  i18n_Language.rb
  Window_TPtL.rb
  Settings.rb
  Input.rb
  Dynamic_Light.rb
  Gamepad_Colors.rb
  Dynamic_Resolution.rb)

find_program(RUBY_EXE ruby
  DOC "Location of the Ruby executable")
set(game_dir "${CMAKE_BINARY_DIR}/bin")
set(data_dir "${game_dir}/Data")
set(xscripts "${data_dir}/xScripts.rxdata")
add_custom_target(data_dir ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory "${data_dir}")
add_custom_command(
  OUTPUT "${xscripts}"
  COMMAND "${RUBY_EXE}" "${CMAKE_SOURCE_DIR}/rpgscript.rb" "${CMAKE_CURRENT_LIST_DIR}" "${game_dir}"
  DEPENDS _scripts.txt ${scripts} data_dir
  COMMENT "Bundling xScripts.rxdata")
add_custom_target(xScripts ALL DEPENDS "${xscripts}")
