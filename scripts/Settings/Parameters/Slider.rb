class Window_Settings
  class SliderParameter < IntParameter
    TYPE = :slider

    def initialize(settings_content, screen_id, position, name: "", additional_icons: [], icons: [], parameter: nil, init_value: 0, min: 0, max: 100)
      super(settings_content, screen_id, position, name: name, additional_icons: additional_icons, icons: icons, parameter: parameter, init_value: init_value, min: min, max: max)
    end

    def get_display_value()
      percent = (self.value - @min_value) * 100 / @max_value
      percent.to_s + "%" 
    end

    def redraw()
      @sprite.bitmap.clear
      redraw_icons
      @sprite.bitmap.font.color = Color.new(255, 255, 255)

      redraw_title

      percent = (self.value.to_f - @min_value.to_f) / @max_value.to_f
      width = (@value_width * percent).to_i
      @sprite.bitmap.fill_rect(Rect.new(@sprite.bitmap.width - @value_width, 4, @value_width, @sprite.bitmap.height - 8), Color.new(255, 255, 255, 64))
      #@sprite.bitmap.fill_rect(Rect.new(@sprite.bitmap.width - @value_width + 1, 5, @value_width - 2, @sprite.bitmap.height - 10), Color.new(255, 255, 255, 0))
      @sprite.bitmap.fill_rect(Rect.new(@sprite.bitmap.width - @value_width, 4, width, @sprite.bitmap.height - 8), Color.new(255, 255, 255, 255))

      if percent >= 0.5
        @sprite.bitmap.font.color = Color.new(0, 0, 0)
        @sprite.bitmap.draw_text(@sprite.bitmap.width - @value_width, 4, width, @sprite.bitmap.height - 8, get_display_value, 1)
      else
        @sprite.bitmap.font.color = Color.new(255, 255, 255)
        @sprite.bitmap.draw_text(@sprite.bitmap.width - @value_width + width, 4, @value_width - width, @sprite.bitmap.height - 8, get_display_value, 1)
      end

      redraw_separator
    end
  end
end