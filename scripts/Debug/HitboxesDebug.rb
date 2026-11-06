class HitboxesDebug < Sprite
  def initialize(viewport)
    super(viewport)
    self.bitmap = Bitmap.new(64, 64)
    self.origin_x = self.bitmap.width / 2
    self.origin_y = self.bitmap.height / 2
    self.x = Graphics.width / 2
    self.y = Graphics.height / 2
    self.order = 20000
  end

  def update
    return if self.disposed? || !self.visible

    (0...self.bitmap.width).each do |pixel_x|
      (0...self.bitmap.height).each do |pixel_y|
        dir = $game_player.direction
        real_x = $game_player.real_x + (pixel_x - self.bitmap.width / 2) * 4
        real_y = $game_player.real_y + (pixel_y - self.bitmap.height / 2) * 4
        x, y = $game_player.real_pos_to_tile_pos(real_x, real_y)
        move_x = dir == 6 ? 1 : dir == 4 ? -1 : 0
        move_y = dir == 8 ? 1 : dir == 2 ? -1 : 0
        bitmap.set_pixel(pixel_x, pixel_y,
          Color.new(
            $game_player.smooth_passable_at?(x, y, real_x, real_y, move_x, move_y) ? 0 : 255,
            ((real_x.to_i - 64) % 128 <= 4 || (real_y.to_i - 64) % 128 <= 4) ? 255 : 0,
            $game_player.smooth_passable_at?(x, y, real_x, real_y, 0, 0, true) ? 0 : 255
          )
        )
      end
    end
  end
end