#==============================================================================
# ** Main
#------------------------------------------------------------------------------
#  After defining each class, actual processing begins here.
#==============================================================================

at_exit do
  RPG::Mod.exec_hooks("hooks/Main/at_exit", binding)
  Wallpaper.reset
  save unless $game_switches[99] || ($game_system.map_interpreter.running? || !$scene.is_a?(Scene_Map))
end

begin
  RPG::Mod.exec_hooks("hooks/Main/start", binding)
  $console = Graphics.fullscreen
  Graphics.frame_rate = 60 
  Font.default_size = 20
  Input.set_led(0, 255, 0)

  #if defined?(RubyVM::YJIT)
  #  RubyVM::YJIT.enable
  #elsif defined?(RubyVM::ZJIT)
  #  RubyVM::ZJIT.enable
  #elsif defined?(RubyVM::RJIT)
  #  RubyVM::RJIT.enable
  #end
  
  # Load persistent data
  Persistent.load

  # Prepare for transition
  Graphics.freeze
  # Make scene object (title screen)
  $scene = Scene_Title.new
  Oneshot.allow_exit false
  Oneshot.exiting false

  # Call main method as long as $scene is effective
  while $scene != nil
    $scene.main
  end
  # Fade out
  Oneshot.exiting true
  Graphics.transition(20)

  if Journal.active?
    Journal.set ''
  end

  if $debug
	tp.disable
  end
  
  Oneshot.allow_exit true
rescue Errno::ENOENT
  if $debug
	tp.disable
  end
  # Supplement Errno::ENOENT exception
  # If unable to open file, display message and end
  filename = $!.message.sub("No such file or directory - ", "")
  print("Unable to find file #{filename}.")
end
