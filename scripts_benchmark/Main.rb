class TestSprite < Sprite
  attr_accessor :direction_x
  attr_accessor :direction_y
  attr_accessor :bitmap1
  attr_accessor :bitmap2

  def initialize(viewport)
    super(viewport)
  
    @direction_x = 0
    @direction_y = 0
    @bitmap1 = nil
    @bitmap2 = nil
    @time = 40
    @u = false
  end
  
  def update
  	return unless bitmap
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
  Graphics.frame_rate = 5000
  Font.default_size = 20
  Oneshot.allow_exit true

  sprites = []
  bitmap1 = RPG::Cache.face("niko_smile")
  bitmap2 = RPG::Cache.face("niko_speak")
  viewport = Viewport.new()
  time = 0
  while true
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
rescue Errno::ENOENT => e
  Sunshine.SetCrashScreenData "#{e.message}"
rescue StandardError => e
  Sunshine.SetCrashScreenData "#{e.class}: #{e.message}"
  e.backtrace.each do |line|
    puts line
  end
ensure
  Oneshot.exiting true
end
