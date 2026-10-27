class Sprite_Footprint < Sprite
  def initialize(viewport, direction, x, y, character_name)
    super(viewport)
    @direction = direction
    @real_x = x * 4 * 32
    @real_y = y * 4 * 32

    @opacity = 255.0

    self.scale_x = 2
    self.scale_y = 2
    self.origin_x = 8
    self.origin_y = 16
    self.bitmap = RPG::Cache.misc('footprints')
    self.src_rect.set(0, 16 * (direction / 2 - 1), 16, 16)
    update
  end

  def update
    return if disposed?
	
    self.x = (@real_x - $game_map.display_x).ceil / 4 + 16
    self.y = (@real_y - $game_map.display_y).ceil / 4 + 32

    @opacity -= 4.0 * Graphics.scaled_delta
    self.opacity = @opacity
    if @opacity <= 0
      dispose
    end
  end

  def correctX(xDelta)
    @real_x += xDelta*4*32
  end
  def correctY(yDelta)
    @real_y += yDelta*4*32
  end
end
