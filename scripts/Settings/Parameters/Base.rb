#PRIORITY 10

class Window_Settings
  class BaseParameter
    TYPE = :base
    attr_accessor :disabled
    attr_reader :parameter

    def self.descendants
      subclasses + subclasses.flat_map(&:descendants)
    end

    def self.for(type, *args, **kwargs)
      if type == TYPE
        return BaseParameter.new(*args, **kwargs)
      end

      target_class = descendants.find { |klass| klass::TYPE == type }

      unless target_class
        puts "WARNING: Unknown parameter type :#{type}"
        return nil
      end

      target_class.new(*args, **kwargs)
    end

    def initialize(settings_content, screen_id, position, name: "", additional_icons: [], icons: [], parameter: nil, init_value: nil)
      @settings_content = settings_content
      @screen_id = screen_id
      @position = position
      @name = name.to_s || ""
      @value = Settings[parameter] || init_value
      @parameter = parameter
      @opacity = 1.0
      @icons = icons || []
      @additional_icons = additional_icons || []
      @disabled = false

      if parameter && Settings[parameter] == nil
        puts "WARNING: Setting was registered for a parameter when the parameter itself does not exist (Parameter :#{parameter}, Screen ID #{screen_id}, Position #{position})"
      end

      @sprite = Sprite.new(@settings_content.viewport)
      @sprite.bitmap = Bitmap.new(PARAMETER_WIDTH + additional_icons.length * ICON_SCALED, PARAMETER_HEIGHT + 1)
      @sprite.bitmap.font.size = 20

      Language.register_text_sprite("settings_parameter #{screen_id}-#{position}", @sprite)

      @value_width = PARAMETER_VALUE_WIDTH
      self.value = self.value # appling init settings
      redraw
    end

    def x
      @sprite.x
    end
    def y
      @sprite.y
    end
    def width
      @sprite.bitmap.width
    end
    def height
      @sprite.bitmap.height
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

    def redraw_icons()
      @additional_icons.each_with_index do |icon_position, index|
        @sprite.bitmap.stretch_blt(Rect.new(index * ICON_SCALED, 0, ICON_SCALED, ICON_SCALED), @settings_content.icons_atlas, Rect.new(icon_position[0] * ICON_SIZE, icon_position[1] * ICON_SIZE, ICON_SIZE, ICON_SIZE))
      end
      @icons.each_with_index do |icon_position, index|
        @sprite.bitmap.stretch_blt(Rect.new((@additional_icons.length + index) * ICON_SCALED, 0, ICON_SCALED, ICON_SCALED), @settings_content.icons_atlas, Rect.new(icon_position[0] * ICON_SIZE, icon_position[1] * ICON_SIZE, ICON_SIZE, ICON_SIZE))
      end
    end

    def offset
      (@additional_icons.length + @icons.length) * ICON_SCALED# + ICON_SIZE / 2
    end

    def redraw_title
      @sprite.bitmap.draw_text(offset, 0, @sprite.bitmap.width - @value_width - offset, @sprite.bitmap.height, tr(@name))
    end

    def redraw_value
      @sprite.bitmap.draw_text(@sprite.bitmap.width - @value_width, 0, @value_width, @sprite.bitmap.height, tr(get_display_value), 2)
    end

    def redraw_separator
      if (@settings_content.need_draw_line(@screen_id, @position))
        @sprite.bitmap.fill_rect(Rect.new(offset, PARAMETER_HEIGHT - 1, PARAMETER_WIDTH, 2), Color.new(255, 255, 255, 24))
      end
    end

    def redraw()
      @sprite.bitmap.clear
      redraw_icons
      redraw_title
      redraw_value
      redraw_separator
    end

    def update()
      disabled_opacity = @disabled ? 0.5 : 1

      @sprite.x = @screen_id * Graphics.width - @settings_content.visible_x - @additional_icons.length * ICON_SCALED + @settings_content.x
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