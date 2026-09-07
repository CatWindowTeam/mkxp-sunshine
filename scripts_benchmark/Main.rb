class TestSprite < Sprite
  attr_accessor :direction_x
  attr_accessor :direction_y
  attr_accessor :bitmap1
  attr_accessor :bitmap2

  def initialize()
    @direction_x = @direction_y = 0
    @bitmap1 = @bitmap2 = nil
    @time = 40
    @u = false
    super
  end
  
  def update
    if (self.x < 0 && @direction_x < 0) || (self.x > Graphics.width - self.bitmap.width && @direction_x > 0)
      @direction_x = -@direction_x
    end
    if (self.y < 0 && @direction_y < 0) || (self.y > Graphics.height - self.bitmap.height && @direction_y > 0)
      @direction_y = -@direction_y
    end

    x += @direction_x
    y += @direction_y

    @time -= 1
    if @time <= 0
      @time = 40
      self.bitmap = @u ? @bitmap1 : @bitmap2
      @u = !@u
    end
  end
end

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

  # Make scene object (title screen)
  $scene = Scene_Title.new
  Oneshot.allow_exit false
  Oneshot.exiting false

  sprites = []
  bitmap1 = RPG::Cache.face("niko_smile")
  bitmap2 = RPG::Cache.face("niko_speak")

  viewport = Viewport.new()

  time = 0

  finish_time = CTime.now + 5 * 60
  while CTime.now < finish_time
    time += 1
    sprite = TestSprite.new(viewport)
    sprite.bitmap1 = bitmap1
    sprite.bitmap2 = bitmap2
    sprite.x = Graphics.width / 2
    sprite.y = Graphics.height / 2
    sprite.direction_x = Math.sin(time / 20 * Math::PI)
    sprite.direction_y = Math.cos(time / 20 * Math::PI)
    sprites << sprite;

    sprites.each do |sprite|
      sprite.update
    end

    # Update Screen
    Graphics.update
  end

  # Prepare for transition
  Graphics.freeze
  
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
