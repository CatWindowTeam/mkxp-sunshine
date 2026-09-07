begin
  RPG::Mod.exec_hooks("hooks/Main/start", binding)
  $console = Graphics.fullscreen
  Graphics.frame_rate = 60
  Font.default_size = 20
  #debug shit 
  Input.set_led(255, 150, 30)
  # Registering parameters
  Settings.reset!
  # Load persistent data
  Persistent.load

  # Prepare for transition
  Graphics.freeze
  # Make scene object (title screen)
  $scene = Scene_Title.new
  Oneshot.allow_exit false
  Oneshot.exiting false

  finish_time = Time.now + 5 * 60
  while CTime.now < finish_time
    puts "TEST PUTS"
    Logger.Debug "TEST LOGS DEBUG"
    Logger.Info "TEST LOGS INFO"
    Logger.Warn "TEST LOGS WARN"
    Logger.Error "TEST LOGS ERROR"
  end
  save unless $game_switches[99] || ($game_system.map_interpreter.running? || !$scene.is_a?(Scene_Map))
rescue Errno::ENOENT => e
  Sunshine.SetCrashScreenData "#{e.message}"
rescue StandardError => e
  Sunshine.SetCrashScreenData "#{e.class}: #{e.message}"
  e.backtrace.each do |line|
    puts line
  end
ensure
  # Fade out
  Oneshot.exiting true
  Graphics.transition(20)
  if Journal.active?
    Journal.set 'default'
  end
  Oneshot.allow_exit true
  Wallpaper.reset
end
