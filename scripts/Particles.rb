class Particle
  def initialize(viewport, bitmap)
    @sprite = Sprite.new(viewport)
    @sprite.bitmap = bitmap
    @sprite.ox = bitmap.width / 2
    @sprite.oy = bitmap.height / 2
    @light = Sprite.new(viewport)
    @light.bitmap = RPG::Cache.light("light")
    @light.ox = @light.bitmap.width / 2
    @light.oy = @light.bitmap.height / 2
    @light.z -= 1
    @light.blend_type = 3
    @light.tone = Tone.new(512, 512, 512)
    @light_power = 1.0
    self.x = rand(Graphics.width)
    self.y = rand(Graphics.height)
  end

  def light_scale
    @light.zoom_x
  end
  def light_scale=(val)
    @light.zoom_x = @light.zoom_y = val
  end

  # Link various things to the sprite
  def x
    @x
  end
  def x=(val)
    @x = val
    width = [@sprite.bitmap.width * @sprite.zoom_x, @light.bitmap.width * @light.zoom_x].max
    if @x < -width
      @x = Graphics.width + width
    elsif @x > Graphics.width + width
      @x = -width
    end
    @light.x = @sprite.x = @x
  end
  def y
    @y
  end
  def y=(val)
    @y = val
    height = [@sprite.bitmap.height * @sprite.zoom_y, @light.bitmap.height * @light.zoom_y].max
    if @y < -height
      @y = Graphics.height + height
    elsif @y > Graphics.height + height
      @y = -height
    end
    @light.y = @sprite.y = @y
  end
  def scale
    @sprite.zoom_x
  end
  def scale=(val)
    @sprite.zoom_x = val
    @sprite.zoom_y = val
  end
  def opacity
    @sprite.opacity
  end
  def opacity=(val)
    @sprite.opacity = val
    val = val.clamp(0.0, 200.0)
    @light.modulate.set(val * @light_power, val * @light_power, val * @light_power)
  end
  def dispose
    @sprite.dispose
    @sprite = nil
  end
end

class Particle_Firefly < Particle
  @@bitmap = RPG::Cache.picture('firefly')

  def initialize(viewport)
    super(viewport, @@bitmap)
    @phase = rand(120)
    @wavelength = rand(120..240)
    @vx = rand(0.2..1.5) * (rand(2) * 2 - 1)
    @vy = rand(0.2..1.5) * (rand(2) * 2 - 1)
    @light_power = 1.0
    self.light_scale = 0.2
    self.scale = rand(0.02..0.08)
    @sprite.blend_type = 1
  end

  def update
    self.x += @vx
    self.y += @vy
    self.opacity = Math.sin((@phase / @wavelength.to_f) * Math::PI) * 255
    @phase = (@phase + 1) % @wavelength
  end
end

# A layer of moving particle objects, useful for fireflies and shrimp
class ParticleLayer
  def initialize(viewport, klass, count)
    @particles = Array.new(count) do
      klass.new(viewport)
    end
    @last_map_x = $game_map.display_x / 4
    @last_map_y = $game_map.display_y / 4
  end

  def update
    return unless @particles
    map_x = $game_map.display_x / 4
    map_y = $game_map.display_y / 4
    @particles.each do |p|
      p.x += @last_map_x - map_x
      p.y += @last_map_y - map_y
      p.update
    end
    @last_map_x = map_x
    @last_map_y = map_y
  end

  def dispose
    return unless @particles
    @particles.each do |p|
      p.dispose
    end
    @particles = nil
  end
end

class Particle_Shrimp < Particle
  @@bitmap = RPG::Cache.picture('shrimp')
  TAU = Math::PI * 2

  def initialize(viewport)
    super(viewport, @@bitmap)
    @angle = rand(0..TAU)
    @speed = rand(0.2..4.0)
    self.light_scale = 0.2
    self.scale = rand(0.04..0.08)
  end

  def update
    case rand(0..10)
    when 0..1
      @angle += rand(-TAU / 8..TAU / 8)
    when 2
      @speed += rand(1.0..5.0)
      @speed = 5.0 if @speed > 5.0
    when 3..4
      @speed -= rand(1.0..2.0)
      @speed = 0.2 if @speed < 0.2
    else
      # no-op
    end
    vx = Math.cos(@angle) * @speed
    vy = Math.sin(@angle) * @speed
    self.x += vx
    self.y += vy
  end
end
