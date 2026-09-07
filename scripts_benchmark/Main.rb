class TestSprite < Sprite
  attr_accessor :direction_x
  attr_accessor :direction_y
  attr_accessor :bitmap1
  attr_accessor :bitmap2
  attr_accessor :x_r
  attr_accessor :y_r

  def initialize(viewport)
    super(viewport)
  
    @direction_x = 0.0
    @direction_y = 0.0
    @bitmap1 = nil
    @bitmap2 = nil
    @time = 0
    @u = false

    self.bitmap = Bitmap.new(96, 96)
    
    @x_r = 0.0
    @y_r = 0.0
  end
  
  def update
    @time -= 1
    if @time <= 0
      @time = 40
      self.bitmap.stretch_blt(Rect.new(0, 0, 96, 96), @u ? @bitmap1 : @bitmap2, Rect.new(0, 0, 96, 96))
      @u = !@u
    end

    if (self.x < 0 && @direction_x < 0) || (self.x > (Graphics.width - self.bitmap.width) && @direction_x > 0)
      @direction_x = -@direction_x
    end
    if (self.y < 0 && @direction_y < 0) || (self.y > (Graphics.height - self.bitmap.height) && @direction_y > 0)
      @direction_y = -@direction_y
    end

    @x_r += @direction_x
    @y_r += @direction_y
    self.x = @x_r
    self.y = @y_r
  end
end

begin
  Graphics.frame_rate = 5000
  Graphics.frameskip = false
  Font.default_size = 10
  Oneshot.allow_exit true
  Audio.bgm_play("Audio/BGM/OnLittleCatFeet")
  sprites = []
  bitmap1 = RPG::Cache.face("niko_smile")
  bitmap2 = RPG::Cache.face("niko_speak")
  viewport = Viewport.new()
  time = 0.0

  debug = Sprite.new(viewport)
  debug.z = 10
  debug.bitmap = Bitmap.new(Graphics.width, Graphics.height)

  while true
    time += 1.0
    sprite = TestSprite.new(viewport)
    sprite.bitmap1 = bitmap1
    sprite.bitmap2 = bitmap2
    sprite.x_r = Graphics.width / 2
    sprite.y_r = Graphics.height / 2
    sprite.direction_x = Math.sin(time / 60.0)
    sprite.direction_y = Math.cos(time / 60.0)
    sprite.z = -time
    sprite.opacity = 1
    sprite.blend_type = 1
    sprite.shader = Shader::WorldMachine
    sprites << sprite;

    sprites.each do |s|
      s.update
    end

    debug.bitmap.clear
    debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 10), "#{sprites.length}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 30), "#{MKXP.data_directory}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 50), "Ruby #{RUBY_VERSION}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 70), "SDL #{Sunshine::SDLVersion_major}.#{Sunshine::SDLVersion_minor}.#{Sunshine::SDLVersion_micro}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 90), "#{Graphics.frame_count}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 110), "#{Graphics.frame_rate}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 130), "#{Graphics.brightness}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 150), "#{Graphics.x}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 170), "#{Graphics.y}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 190), "#{Journal.active?}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 210), "#{Oneshot::USER_NAME}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 230), "#{Oneshot::OS}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 240), "#{Oneshot::DE}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 260), "#{Oneshot::SAVE_PATH}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 280), "#{Oneshot::DOCS_PATH}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 300), "#{Oneshot::GAME_PATH}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 320), "#{Oneshot::JOURNAL}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 340), "#{Oneshot::LANG}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 360), "#{Steam.enabled?}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 380), "#{Sunshine::VERSION}")
	debug.bitmap.draw_text(Rect.new(0, 0, Graphics.width, 400), "#{Sunshine::P_LOCALE}")

	
    # Update Screen
    Graphics.update
    if sprites.length > 2000
      sprites.each do |s|
        s.dispose
        s = nil
      end
      sprites = []
    end  
  end
rescue Errno::ENOENT => e
  Sunshine.SetCrashScreenData "#{e.message}"
rescue StandardError => e
  Sunshine.SetCrashScreenData "#{e.class}: #{e.message}"
  e.backtrace.each do |line|
    puts line
  end
ensure
  Audio.bgm_stop
  Oneshot.exiting true
end
