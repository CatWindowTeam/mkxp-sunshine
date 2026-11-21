class Sprite_Debug < Sprite
  def initialize
    super
    self.bitmap = Bitmap.new(Graphics.width, Graphics.height)
    self.bitmap.font.size = 16
    self.order = 500
    self.bitmap.font.color.set(255, 255, 255)
    update
  end

  def dispose
    if self.bitmap != nil
      self.bitmap.dispose
    end
    super
  end

  def update
    super
    self.visible = Settings[:debug]
    if Settings[:debug]
      self.bitmap.clear
      self.bitmap.draw_text(5, 15, Graphics.height, 10, "Msg text: #{$game_temp.message_text}")
      self.bitmap.draw_text(5, 15 * 2, Graphics.height, 10, "Msg text unmod: #{$game_temp.message_text_unmodifed}")
      self.bitmap.draw_text(5, 15 * 3, Graphics.height, 10, "Voiceover file: #{$game_temp.message_text_voiceover}")
      self.bitmap.draw_text(5, 15 * 4, Graphics.height, 10, "Common event ID: #{$game_temp.common_event_id}")
      self.bitmap.draw_text(5, 15 * 5, Graphics.height, 10, "Tileset name: #{$game_map.tileset_name}")
      self.bitmap.draw_text(5, 15 * 6, Graphics.height, 10, "Autotile name: #{$game_map.autotile_names}")
      self.bitmap.draw_text(5, 15 * 7, Graphics.height, 10, "display x: #{$game_map.display_x}")
      self.bitmap.draw_text(5, 15 * 8, Graphics.height, 10, "display y: #{$game_map.display_y}")
    end
  end
end
