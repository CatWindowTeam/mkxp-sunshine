class Window_Settings
  SCREENS_PANELS_MARGIN = 32
  PARAMETER_WIDTH = (Graphics.width - 640) / 2 + 512
  PARAMETER_HEIGHT = 32
  PARAMETER_VALUE_WIDTH = 200

  class SettingsContent
    attr_reader :x
    attr_reader :screen
    attr_reader :index
    attr_reader :viewport
    attr_reader :offset

    def initialize(viewport, offset_y)
      @viewport = viewport
      @screen = 0
      @index = 0
      @opacity = 255
      @offset = offset_y

      @visible_x = 0
      @visible_y = offset_y
      @x = (Graphics.width - PARAMETER_WIDTH) / 2 - 8
      @y = offset_y

      @selection_sprite = Sprite.new(@viewport)
      @selection_sprite.bitmap = Bitmap.new(PARAMETER_WIDTH + 16, PARAMETER_HEIGHT)
      @selection_sprite.bitmap.fill_rect(Rect.new(0, 0, PARAMETER_WIDTH + 16, PARAMETER_HEIGHT), Color.new(255, 255, 255, 64))
      @selection_sprite.x = @x
      @selection_sprite.y = @offset
      @selection_sprite.opacity = 0
      @selection_sprite.blend_type = 1
      @selection_sprite.z = 1

      @screen_panel_selection_sprite = Sprite.new(@viewport)
      @screen_panel_selection_sprite.bitmap = Bitmap.new(1, @offset - SCREENS_PANELS_MARGIN * 2)
      @screen_panel_selection_sprite.bitmap.gradient_fill_rect(0, 0, @screen_panel_selection_sprite.bitmap.width, @screen_panel_selection_sprite.bitmap.height, Color.new(255, 255, 255, 0), Color.new(255, 255, 255, 128), true)
      @screen_panel_selection_sprite.y = SCREENS_PANELS_MARGIN
      @screen_panel_selection_sprite.x = Graphics.width / 2

      @parameters = {}
      @screen_panels = []
      @next_screen_panel_offset = 0
    end
    
    def dispose
      @selection_sprite.dispose
      @screen_panel_selection_sprite.dispose
      @parameters.each do |screen_name, screen_parameters|
        screen_parameters.each do |parameter|
          parameter.dispose
        end
      end
      @screen_panels.each do |screen_panel|
        screen_panel.dispose
      end
    end

    # registering
    def add_screen(name)
      @parameters[name] = []

      screen_panel = Sprite.new(@viewport)
      screen_panel.bitmap = Bitmap.new(name.length * 10 + SCREENS_PANELS_MARGIN, @offset - SCREENS_PANELS_MARGIN * 2)
      screen_panel.bitmap.draw_text(SCREENS_PANELS_MARGIN / 2, 0, screen_panel.bitmap.width, screen_panel.bitmap.height, name, 1)
      screen_panel.y = SCREENS_PANELS_MARGIN
      screen_panel.x = Graphics.width / 2 + @next_screen_panel_offset - (@screen_panels[0] ? @screen_panels[0].width / 2 : screen_panel.width / 2)
      @next_screen_panel_offset += screen_panel.width
      @screen_panels << screen_panel
    end

    def add_parameter(sreen_name, parameter)
      @parameters[sreen_name] << parameter
    end
    
    def get_parameter(screen_name, parameter_id)
      @parameters&.[](screen_name)&.[](parameter_id)
    end
    def get_parameter_by_sceen_id(screen_id, parameter_id)
      @parameters&.values&.[](screen_id)&.[](parameter_id)
    end
    def get_current_parameter
      @parameters&.values&.[](@screen)&.[](@index)
    end

    # updating
    def update()
      screen_result_pos = @screen * Graphics.width
      if (@visible_x - screen_result_pos).abs <= 1
        @visible_x = screen_result_pos
      end
      @visible_x = screen_result_pos * 0.3 + @visible_x * 0.7
      @visible_y = @y * 0.5 + @visible_y * 0.5

      selection_sprite_result_pos = @index * PARAMETER_HEIGHT + @y
      if (@selection_sprite.y - selection_sprite_result_pos).abs <= 1
        @selection_sprite.y = selection_sprite_result_pos
      end
      @selection_sprite.y = selection_sprite_result_pos * 0.5 + @selection_sprite.y * 0.5

      @parameters.each do |screen_name, screen_parameters|
        screen_parameters.each do |parameter|
          parameter.update
        end
      end

      # top panels updates
      to_current_panel = 0
      @screen_panels.each_with_index do |screen_panel, index|
        if index < @screen
          to_current_panel += screen_panel.width
        end
      end

      screen_panel_offset = 0
      @screen_panels.each do |screen_panel|
        res = @screen_panel_selection_sprite.x - to_current_panel + screen_panel_offset - SCREENS_PANELS_MARGIN / 2
        if (screen_panel.x - res).abs <= 3
          screen_panel.x = res
        end
        screen_panel.x = res * 0.3 + screen_panel.x * 0.7
        screen_panel_offset += screen_panel.width

        screen_panel.opacity = ((@opacity.to_f / 255.0) * (1.0 - (Graphics.width / 2 - screen_panel.x - screen_panel.width / 2).abs.to_f / 300.0) * 255.0).to_i
      end
      
      @screen_panel_selection_sprite.zoom_x = @screen_panels[@screen].width * 0.3 + @screen_panel_selection_sprite.zoom_x * 0.7
      @screen_panel_selection_sprite.x = (Graphics.width / 2 - @screen_panels[@screen].width * 0.5) * 0.3 + @screen_panel_selection_sprite.x * 0.7
    end

    def redraw_all
      @parameters.each do |screen_name, screen_parameters|
        screen_parameters.each do |parameter|
          parameter.redraw
          parameter.update
        end
      end
    end

    def update_pos()
      center = Graphics.height / 2 - PARAMETER_HEIGHT * 0.5
      @y = @offset - (@index * PARAMETER_HEIGHT - center + @offset).clamp(0, [0, (height - center * 2 - PARAMETER_HEIGHT * 0.5 + @offset)].max)
    end

    # Navigation
    def screen_left()
      @screen = (@screen - 1) % @parameters.length
      @index = @index.clamp(0, @parameters.values[@screen].length - 1)
      update_pos
    end
    def screen_right()
      @screen = (@screen + 1) % @parameters.length
      @index = @index.clamp(0, @parameters.values[@screen].length - 1)
      update_pos
    end

    def parameter_up()
      offset = 1
      if @parameters.values&.[](@screen)&.[](@index - 1)&.class&.const_get(:TYPE) == :sep
        offset += 1
      end
      @index = (@index - offset) % @parameters.values[@screen].length
      update_pos
    end
    def parameter_down()
      offset = 1
      if @parameters.values&.[](@screen)&.[](@index + 1)&.class&.const_get(:TYPE) == :sep
        offset += 1
      end
      @index = (@index + offset) % @parameters.values[@screen].length
      update_pos
    end

    def parameter_select_offset(offset)
      @index = (@index + offset) % @parameters.values[@screen].length
      update_pos
    end
    
    # Parameters set
    def parameter_left()
      @parameters.values[@screen][@index].value_left
    end
    def parameter_right()
      @parameters.values[@screen][@index].value_right
    end

    # other
    def need_draw_line(screen_id, index)
      klass = @parameters.values&.[](screen_id)&.[](index + 1)&.class
      if klass&.const_get(:TYPE) == :sep || klass == nil
        return false
      end
      return true
    end

    def height
      return @parameters.values[@screen].length * PARAMETER_HEIGHT
    end

    def visible_x()
      @visible_x
    end
    def visible_y()
      @visible_y
    end

    def opacity
      @opacity
    end
    def opacity=(value)
      @opacity = value
      @selection_sprite.opacity = value
      @screen_panel_selection_sprite.opacity = value
      redraw_all
    end
  end

  # ----------------------------------------------------------------------------------------
  class BaseParameter
    TYPE = :base

    def initialize(settings_content, screen_id, position, name, parameter, init_value)
      @settings_content = settings_content
      @screen_id = screen_id
      @position = position
      @name = name.to_s || ""
      @value = Settings[parameter] || init_value
      @parameter = parameter
      @opacity = 1.0

      @sprite = Sprite.new(@settings_content.viewport)
      @sprite.bitmap = Bitmap.new(PARAMETER_WIDTH, PARAMETER_HEIGHT + 1)
      @sprite.bitmap.font.size = 20

      redraw()
    end
    
    def opacity
      @opacity
    end
    def opacity=(value)
      @opacity = value.to_f / 255.0
      redraw
    end

    def value
      @value
    end
    def value=(value)
      @value = value
      if @parameter != nil && Settings::Setters.respond_to?(@parameter)
        Settings::Setters.send(@parameter, @value)
      end
      Settings[@parameter] = @value
    end

    def name
      @name
    end
    def name=(name)
      @name = name
      redraw()
    end

    def get_display_value()
      @value.to_s || "null"
    end

    def redraw()
      @sprite.bitmap.clear
      @sprite.bitmap.draw_text(0, 0, @sprite.bitmap.width - PARAMETER_VALUE_WIDTH, @sprite.bitmap.height, @name)
      @sprite.bitmap.draw_text(@sprite.bitmap.width - PARAMETER_VALUE_WIDTH, 0, PARAMETER_VALUE_WIDTH, @sprite.bitmap.height, get_display_value, 2)
      if (@settings_content.need_draw_line(@screen_id, @position))
        @sprite.bitmap.fill_rect(Rect.new(0, PARAMETER_HEIGHT - 1, PARAMETER_WIDTH, 2), Color.new(255, 255, 255, 24))
      end
    end

    def update()
      @sprite.x = @screen_id * Graphics.width - @settings_content.visible_x + @settings_content.x
      @sprite.y = @settings_content.visible_y + @position * PARAMETER_HEIGHT
      @sprite.opacity = ((1.0 + ((@sprite.y - @settings_content.offset).to_f / 64.0).clamp(-1.0, 0.0)) * @opacity * (@settings_content.opacity.to_f / 255.0) * 255.0).to_i 
    end

    def dispose()
      @sprite.dispose
    end

    def value_left() end
    def value_right() end
    def action() end
  end

  # ----------------------------------------------------------------------------------------
  class Separator < BaseParameter
    TYPE = :sep

    def initialize(settings_content, screen_id, position, name)
      super(settings_content, screen_id, position, name, nil, "")
    end
    
    def redraw()
      @sprite.bitmap.clear
      @sprite.bitmap.fill_rect(Rect.new(0, PARAMETER_HEIGHT / 2 - 1, PARAMETER_WIDTH, 2), Color.new(255, 255, 255, 64))
      if (@name != "")
        @sprite.bitmap.clear_rect(Rect.new((PARAMETER_WIDTH - @name.length * 10) / 2 - 8, 0, @name.length * 10 + 16, PARAMETER_HEIGHT))
        @sprite.bitmap.draw_text(0, 0, @sprite.bitmap.width, @sprite.bitmap.height, @name, 1)
      end
    end
  end

  # ----------------------------------------------------------------------------------------
  class BoolParameter < BaseParameter
    TYPE = :bool

    def initialize(settings_content, screen_id, position, name, parameter, bool_value)
      super(settings_content, screen_id, position, name, parameter, bool_value || false)
    end

    def get_display_value
      return @value ? tr("ON") : tr("OFF")
    end
    
    def value_left()
      self.value = !@value
      redraw
    end
    def value_right()
      self.value = !@value
      redraw
    end
    def action()
      self.value = !@value
      redraw
    end
  end

  # ----------------------------------------------------------------------------------------
  class IntParameter < BaseParameter
    TYPE = :int
    
    attr_reader :max_value
    attr_reader :min_value

    def initialize(settings_content, screen_id, position, name, parameter, int_value, min_value, max_value)
      super(settings_content, screen_id, position, name, parameter, int_value)

      @min_value = min_value
      @max_value = max_value
    end

    def value_left()
      self.value = (@value - 1).clamp(@min_value, @max_value)
      redraw
    end
    def value_right()
      self.value = (@value + 1).clamp(@min_value, @max_value)
      redraw
    end
  end

  # ----------------------------------------------------------------------------------------
  class EnumParameter < IntParameter
    TYPE = :enum

    def initialize(settings_content, screen_id, position, name, parameter, int_value, enum_texts)
      super(settings_content, screen_id, position, name, parameter, int_value, 0, enum_texts.length - 1)

      @texts = enum_texts
    end

    def get_display_value
      @texts&.[](@value) || @value.to_s || "null"
    end

    def value_left()
      self.value = (@value - 1) % [1, @max_value + 1].max
      redraw
    end
    def value_right()
      self.value = (@value + 1) % [1, @max_value + 1].max
      redraw
    end
  end
end