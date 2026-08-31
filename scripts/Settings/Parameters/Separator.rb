class Window_Settings
  class Separator < BaseParameter
    TYPE = :sep

    def initialize(settings_content, screen_id, position, name: nil, icons: [])
      super(settings_content, screen_id, position, name: name, additional_icons: [], icons: icons, parameter: nil, init_value: "")
    end
    
    def redraw()
      @sprite.bitmap.clear
      @sprite.bitmap.fill_rect(Rect.new(0, PARAMETER_HEIGHT / 2 - 1, PARAMETER_WIDTH, 2), Color.new(255, 255, 255, 64))
      if (@name && @name != "")
        offset = !!@icon_position ? ICON_SIZE + 8 : 0
        name = tr(@name)
        text_width = @sprite.bitmap.text_size(name).width
        @sprite.bitmap.clear_rect(Rect.new((PARAMETER_WIDTH - text_width - offset) / 2 - 8, 0, text_width + 16 + offset, PARAMETER_HEIGHT))
        @sprite.bitmap.draw_text(offset, 0, @sprite.bitmap.width - offset, @sprite.bitmap.height, name, 1)
      end
    end
  end
end