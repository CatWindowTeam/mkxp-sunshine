class Window_Settings
  class SliderParameter < IntParameter
    TYPE = :slider

    def initialize(settings_content, screen_id, position, name, icon, parameter, int_value, min_value, max_value)
      super(settings_content, screen_id, position, name, icon, parameter, int_value, min_value, max_value)
    end

    def get_display_value()
      percent = (self.value - @min_value) * 100 / @max_value
      percent.to_s + "%" 
    end

    def redraw()
      offset = !!@icon_position ? ICON_SIZE * 2 + 8 : 0
      @sprite.bitmap.clear
      redraw_icon
      @sprite.bitmap.font.color = Color.new(255, 255, 255)

      @sprite.bitmap.draw_text(offset, 0, @sprite.bitmap.width - @value_width - offset, @sprite.bitmap.height, tr(@name))

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

      if (@settings_content.need_draw_line(@screen_id, @position))
        @sprite.bitmap.fill_rect(Rect.new(0, PARAMETER_HEIGHT - 1, PARAMETER_WIDTH, 2), Color.new(255, 255, 255, 24))
      end

      #if @icon_position
      #  @sprite.bitmap.blt(0, (PARAMETER_HEIGHT - ICON_SIZE * 2) / 2, RPG::Cache.menu("icons"), Rect.new(@icon_position[0] * ICON_SIZE, @icon_position[1] * ICON_SIZE, ICON_SIZE, ICON_SIZE))
      #end
    end
  end
end