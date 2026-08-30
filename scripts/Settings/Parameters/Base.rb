#PRIORITY 10

class Window_Settings
  class BaseParameter
    TYPE = :base
    attr_accessor :disabled
    attr_reader :parameter

    def initialize(settings_content, screen_id, position, name, icon, parameter, init_value)
      @settings_content = settings_content
      @screen_id = screen_id
      @position = position
      @name = name.to_s || ""
      if parameter && Settings[parameter] == nil
        puts "WARNING: Setting was registered for a parameter when the parameter itself does not exist (Parameter :#{parameter})"
      end
      @value = Settings[parameter] || init_value
      @parameter = parameter
      @opacity = 1.0
      @icon_position = icon
      @disabled = false

      @sprite = Sprite.new(@settings_content.viewport)
      @sprite.bitmap = Bitmap.new(PARAMETER_WIDTH, PARAMETER_HEIGHT + 1)
      @sprite.bitmap.font.size = 20

      #if @icon_position
      #  @icon = Sprite.new(@settings_content.viewport)
      #  @icon.bitmap = Bitmap.new(ICON_SIZE, ICON_SIZE)
      #  @icon.zoom_x = @icon.zoom_y = 2
      #end

      Language.register_text_sprite("settings_parameter #{screen_id}-#{position}", @sprite)

      @value_width = PARAMETER_VALUE_WIDTH
      self.value = self.value # appling init settings
      redraw
    end
    
    def opacity
      @opacity
    end
    def opacity=(value)
      @opacity = value.to_f / 255.0
    end

    def value
      if @parameter && Settings.has?(@parameter)
        @value = Settings[@parameter]
      end
      @value
    end
    def value=(value)
      if @parameter && (Settings.has?(@parameter))
        Settings[@parameter] = value
        if Settings::Setters.respond_to?(@parameter)
          Settings::Setters.send(@parameter, value)
        end
      end
      @value = value
    end

    def name
      @name
    end
    def name=(name)
      @name = name
      redraw
    end

    def get_display_value()
      self.value.to_s || "null"
    end

    def redraw_icon()
      if @icon_position
        #@icon.bitmap.clear
        #@icon.bitmap.blt(0, 0, RPG::Cache.menu("icons"), Rect.new(@icon_position[0] * ICON_SIZE, @icon_position[1] * ICON_SIZE, ICON_SIZE, ICON_SIZE))
        @sprite.bitmap.stretch_blt(Rect.new(0, 0, 32, 32), RPG::Cache.menu("icons"), Rect.new(@icon_position[0] * ICON_SIZE, @icon_position[1] * ICON_SIZE, ICON_SIZE, ICON_SIZE))
      end
    end

    def redraw()
      offset = !!@icon_position ? ICON_SIZE * 2 + 8 : 0
      @sprite.bitmap.clear
      @sprite.bitmap.draw_text(offset, 0, @sprite.bitmap.width - @value_width - offset, @sprite.bitmap.height, tr(@name))
      @sprite.bitmap.draw_text(@sprite.bitmap.width - @value_width, 0, @value_width, @sprite.bitmap.height, tr(get_display_value), 2)
      if (@settings_content.need_draw_line(@screen_id, @position))
        @sprite.bitmap.fill_rect(Rect.new(0, PARAMETER_HEIGHT - 1, PARAMETER_WIDTH, 2), Color.new(255, 255, 255, 24))
      end
      #if @icon_position
      #  @sprite.bitmap.blt(0, (PARAMETER_HEIGHT - ICON_SIZE * 2) / 2, RPG::Cache.menu("icons"), Rect.new(@icon_position[0] * ICON_SIZE, @icon_position[1] * ICON_SIZE, ICON_SIZE, ICON_SIZE))
      #end
      redraw_icon
    end

    def update()
      disabled_opacity = @disabled ? 0.5 : 1

      @sprite.x = @screen_id * Graphics.width - @settings_content.visible_x + @settings_content.x
      @sprite.y = @settings_content.visible_y + @position * PARAMETER_HEIGHT
      @sprite.opacity = ((1.0 + ((@sprite.y - @settings_content.offset).to_f / 64.0).clamp(-1.0, 0.0)) * @opacity * (@settings_content.opacity.to_f / 255.0) * disabled_opacity * 255.0).to_i

      #if @icon_position
      #  @icon.opacity = (@opacity * (@settings_content.opacity.to_f / 255.0) * disabled_opacity * 255.0)
      #  @icon.x = @sprite.x
      #  @icon.y = @sprite.y - (PARAMETER_HEIGHT - ICON_SIZE * 2) / 2
      #end
    end

    def dispose()
      @sprite.dispose
      #if @icon_position
      #  @icon.dispose
      #end
    end

    def value_left() end
    def value_right() end
    def action() end
    # only for visual updating, its not selecting for real
    def select(previous) end
    def deselect() end
  end
end