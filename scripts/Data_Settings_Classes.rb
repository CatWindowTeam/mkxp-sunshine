# Logic of settings UI
class Window_Settings
  SCREENS_PANELS_MARGIN = 32
  SCREENS_PANELS_TOP_MARGIN = 16
  PARAMETER_WIDTH = (Graphics.width - 640) / 2 + 512
  PARAMETER_HEIGHT = 32
  PARAMETER_VALUE_WIDTH = PARAMETER_WIDTH / 2
  PARAMETER_KEY_WIDTH = PARAMETER_WIDTH / 5
  ICON_SIZE = 16
  # typed sizes
  BOOL_VALUE_WIDTH = 120

  PARAMETER_CHANGE_AUDIO = "Audio/SE/text_robot.wav"

  class SettingsContent
    attr_reader :x
    attr_reader :screen
    attr_reader :index
    attr_reader :viewport
    attr_reader :offset

    def initialize(viewport, offset_y, screen = 0, index = 0)
      Window_Settings.send(:remove_const, :PARAMETER_WIDTH)
      Window_Settings.const_set(:PARAMETER_WIDTH, (Graphics.width - 640) / 2 + 512) # костыль бля

      @viewport = viewport
      @screen = screen
      @index = index
      @opacity = 255
      @offset = offset_y
      @waiting_for_key = false
      @wait_timer = 0

      @visible_x = @screen * Graphics.width
      @visible_y = offset_y
      @x = (Graphics.width - PARAMETER_WIDTH) / 2
      @y = offset_y

      @selection_sprite = Sprite.new(@viewport)
      @selection_sprite.bitmap = Bitmap.new(PARAMETER_WIDTH + 16, PARAMETER_HEIGHT)
      @selection_sprite.bitmap.fill_rect(Rect.new(0, 0, PARAMETER_WIDTH + 16, PARAMETER_HEIGHT), Color.new(255, 255, 255, 64))
      @selection_sprite.x = @x - 8
      @selection_sprite.y = @index * PARAMETER_HEIGHT + @y
      @selection_sprite.blend_type = 1
      @selection_sprite.z = 1

      @screen_panel_selection_sprite = Sprite.new(@viewport)
      @screen_panel_selection_sprite.bitmap = Bitmap.new(1, @offset - SCREENS_PANELS_MARGIN * 2 - SCREENS_PANELS_TOP_MARGIN)
      @screen_panel_selection_sprite.bitmap.gradient_fill_rect(0, 0, @screen_panel_selection_sprite.bitmap.width, @screen_panel_selection_sprite.bitmap.height, Color.new(255, 255, 255, 0), Color.new(255, 255, 255, 128), true)
      @screen_panel_selection_sprite.y = SCREENS_PANELS_MARGIN + SCREENS_PANELS_TOP_MARGIN
      @screen_panel_selection_sprite.x = Graphics.width / 2
      @screen_panel_selection_sprite.blend_type = 1

      @switch_panels_hint_left = Sprite.new(@viewport)
      @switch_panels_hint_left.bitmap = Bitmap.new(PARAMETER_WIDTH / 2 - SCREENS_PANELS_MARGIN, @screen_panel_selection_sprite.height / 2)
      
      @switch_panels_hint_right = Sprite.new(@viewport)
      @switch_panels_hint_right.bitmap = Bitmap.new(@switch_panels_hint_left.bitmap.width, @switch_panels_hint_left.bitmap.height)

      @target_hints_x = @x + SCREENS_PANELS_MARGIN

      @switch_panels_hint_left.bitmap.font.size = @switch_panels_hint_right.bitmap.font.size = 20
      @switch_panels_hint_left.x = @switch_panels_hint_right.x = @target_hints_x
      @switch_panels_hint_left.y = @switch_panels_hint_right.y = @screen_panel_selection_sprite.y
      @switch_panels_hint_left.zoom_x = @switch_panels_hint_right.zoom_x =
      @switch_panels_hint_left.zoom_y = @switch_panels_hint_right.zoom_y = 2
      @switch_panels_hint_left.opacity = @switch_panels_hint_right.opacity = 127
      @switch_panels_hint_left.blend_type = @switch_panels_hint_right.blend_type = 1

      Language.register_text_sprite("settings_switch_panels_hint_left", @switch_panels_hint_left)
      Language.register_text_sprite("settings_switch_panels_hint_right", @switch_panels_hint_right)

      redraw_panels_hints

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
      @switch_panels_hint_left.dispose
      @switch_panels_hint_right.dispose
    end

    # registering
    def add_screen(name)
      @parameters[name] = []

      screen_panel = Sprite.new(@viewport)
      screen_panel.bitmap = Bitmap.new(@switch_panels_hint_left.bitmap.text_size(tr(name)).width + SCREENS_PANELS_MARGIN, @screen_panel_selection_sprite.height)
      screen_panel.bitmap.draw_text(SCREENS_PANELS_MARGIN / 2, 0, screen_panel.width, screen_panel.height, tr(name), 1)
      screen_panel.y = @screen_panel_selection_sprite.y
      screen_panel.x = Graphics.width / 2 + @next_screen_panel_offset - (@screen_panels[0] ? @screen_panels[0].width / 2 : screen_panel.width / 2)
      @next_screen_panel_offset += screen_panel.width
      @screen_panels << screen_panel

      Language.register_text_sprite("settings_screen_panel #{name}", screen_panel)
    end

    def add_parameter(sreen_name, parameter)
      @parameters[sreen_name] << parameter
      parameter
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

    def disable_setting(screen, position)
      if screen.is_a?(String)
        get_parameter(screen, position).disable
      else
        get_parameter_by_sceen_id(screen, position).disable
      end
    end
    def enable_setting(screen, position)
      if screen.is_a?(String)
        get_parameter(screen, position).enable
      else
        get_parameter_by_sceen_id(screen, position).enable
      end
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

        screen_panel.opacity = ((@opacity.to_f / 255.0) * (1.0 - (screen_panel.x + 16 - (Graphics.width - screen_panel.width) / 2).abs.to_f / (PARAMETER_WIDTH / 2 - 48)) * 255.0).to_i
      end
      
      @screen_panel_selection_sprite.zoom_x = @screen_panels[@screen].width * 0.3 + @screen_panel_selection_sprite.zoom_x * 0.7
      @screen_panel_selection_sprite.x = (Graphics.width - @screen_panels[@screen].width) * 0.5 * 0.3 + @screen_panel_selection_sprite.x * 0.7

      @switch_panels_hint_left.x = @switch_panels_hint_left.x * 0.8 + @target_hints_x * 0.2
      @switch_panels_hint_right.x = @switch_panels_hint_right.x * 0.8 + @target_hints_x * 0.2

      @waiting_for_key2 = @waiting_for_key

      @wait_timer = (@wait_timer + (@waiting_for_key ? 1 : -1)).clamp(0, 3)
    end

    def redraw_all
      @parameters.each do |screen_name, screen_parameters|
        screen_parameters.each do |parameter|
          parameter.redraw
          parameter.update
        end
      end
      redraw_panels_hints
      @parameters.keys.each_with_index do |name, index|
        screen_panel = @screen_panels[index]
        screen_panel.bitmap.dispose
        screen_panel.bitmap = Bitmap.new(@switch_panels_hint_left.bitmap.text_size(tr(name)).width + SCREENS_PANELS_MARGIN, @screen_panel_selection_sprite.height)
        screen_panel.bitmap.draw_text(SCREENS_PANELS_MARGIN / 2, 0, screen_panel.width, screen_panel.height, tr(name), 1)
      end
    end
    
    def redraw_panels_hints
      @switch_panels_hint_right.bitmap.clear
      @switch_panels_hint_left.bitmap.clear
      @switch_panels_hint_right.bitmap.draw_text(0, 0, @switch_panels_hint_right.bitmap.width, @switch_panels_hint_right.bitmap.height, "W→", 2)
      @switch_panels_hint_left.bitmap.draw_text(0, 0, @switch_panels_hint_left.bitmap.width, @switch_panels_hint_left.bitmap.height, "←Q")
    end

    def update_pos()
      center = Graphics.height / 2 - PARAMETER_HEIGHT * 0.5
      @y = @offset - (@index * PARAMETER_HEIGHT - center + @offset).clamp(0, [0, (height - center * 2 - PARAMETER_HEIGHT * 0.5 + @offset)].max)
    end

    # Navigation
    def screen_left()
      previous = @index
      @parameters.values[@screen]&.[](@index)&.deselect
      @screen = (@screen - 1) % @parameters.length
      @index = @index.clamp(0, @parameters.values[@screen].length - 1)
      @switch_panels_hint_left.x -= SCREENS_PANELS_MARGIN
      @parameters.values[@screen]&.[](@index)&.select(previous)
      update_pos
      if @parameters.values[@screen]&.[](@index)&.class&.const_get(:TYPE) == :sep
        parameter_down
      end
    end
    def screen_right()
      previous = @index
      @parameters.values[@screen]&.[](@index)&.deselect
      @screen = (@screen + 1) % @parameters.length
      @index = @index.clamp(0, @parameters.values[@screen].length - 1)
      @switch_panels_hint_right.x += SCREENS_PANELS_MARGIN
      @parameters.values[@screen]&.[](@index)&.select(previous)
      update_pos
      if @parameters.values[@screen]&.[](@index)&.class&.const_get(:TYPE) == :sep
        parameter_down
      end
    end

    def parameter_up()
      previous = @index
      @parameters.values[@screen]&.[](@index)&.deselect
      @index = (@index - 1) % @parameters.values[@screen].length
      if @parameters.values&.[](@screen)&.[](@index)&.class&.const_get(:TYPE) == :sep
        @index = (@index - 1) % @parameters.values[@screen].length
      end
      @parameters.values[@screen]&.[](@index)&.select(previous)
      update_pos
    end
    def parameter_down()
      previous = @index
      @parameters.values[@screen]&.[](@index)&.deselect
      @index = (@index + 1) % @parameters.values[@screen].length
      if @parameters.values&.[](@screen)&.[](@index)&.class&.const_get(:TYPE) == :sep
        @index = (@index + 1) % @parameters.values[@screen].length
      end
      @parameters.values[@screen]&.[](@index)&.select(previous)
      update_pos
    end

    def parameter_select_offset(offset)
      previous = @index
      @parameters.values[@screen]&.[](@index)&.deselect
      @index = (@index + offset) % @parameters.values[@screen].length
      if @parameters.values&.[](@screen)&.[](@index)&.class&.const_get(:TYPE) == :sep
        @index += offset <=> 0
      end
      @parameters.values[@screen]&.[](@index)&.select(previous)
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
      @switch_panels_hint_right.opacity = @switch_panels_hint_left.opacity = (value * 127) / 255
      redraw_all
    end

    def waiting_for_key=(value)
      @waiting_for_key = value
    end
    def waiting_for_key
      @wait_timer > 0
    end
  end

  # ----------------------------------------------------------------------------------------
  class BaseParameter
    TYPE = :base

    def initialize(settings_content, screen_id, position, name, icon, parameter, init_value)
      @settings_content = settings_content
      @screen_id = screen_id
      @position = position
      @name = name.to_s || ""
      @value = Settings[parameter] || init_value
      @parameter = parameter
      @opacity = 1.0
      @icon_position = icon
      @disabled = false

      @sprite = Sprite.new(@settings_content.viewport)
      @sprite.bitmap = Bitmap.new(PARAMETER_WIDTH, PARAMETER_HEIGHT + 1)
      @sprite.bitmap.font.size = 20

      if @icon_position
        @icon = Sprite.new(@settings_content.viewport)
        @icon.bitmap = Bitmap.new(ICON_SIZE, ICON_SIZE)
        @icon.zoom_x = @icon.zoom_y = 2
      end

      Language.register_text_sprite("settings_parameter #{screen_id}-#{position}", @sprite)

      @value_width = PARAMETER_VALUE_WIDTH
      self.value = self.value # appling init settings
      redraw
    end

    def enable
      @disabled = false
    end
    def disable
      @disabled = true
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
        @icon.bitmap.clear
        @icon.bitmap.blt(0, 0, RPG::Cache.menu("icons"), Rect.new(@icon_position[0] * ICON_SIZE, @icon_position[1] * ICON_SIZE, ICON_SIZE, ICON_SIZE))
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

      if @icon_position
        @icon.opacity = (@opacity * (@settings_content.opacity.to_f / 255.0) * disabled_opacity * 255.0)
        @icon.x = @sprite.x
        @icon.y = @sprite.y - (PARAMETER_HEIGHT - ICON_SIZE * 2) / 2
      end
    end

    def dispose()
      @sprite.dispose
      if @icon_position
        @icon.dispose
      end
    end

    def value_left() end
    def value_right() end
    def action() end
    # only for visual updating, its not selecting for real
    def select(previous) end
    def deselect() end
  end

  # ----------------------------------------------------------------------------------------
  class Separator < BaseParameter
    TYPE = :sep

    def initialize(settings_content, screen_id, position, name, icon)
      super(settings_content, screen_id, position, name, icon, nil, "")
    end
    
    def redraw()
      redraw_icon
      @sprite.bitmap.clear
      @sprite.bitmap.fill_rect(Rect.new(0, PARAMETER_HEIGHT / 2 - 1, PARAMETER_WIDTH, 2), Color.new(255, 255, 255, 64))
      if (@name && @name != "")
        offset = !!@icon_position ? ICON_SIZE + 8 : 0
        name = tr(@name)
        text_width = @sprite.bitmap.text_size(name).width
        @sprite.bitmap.clear_rect(Rect.new((PARAMETER_WIDTH - text_width - offset) / 2 - 8, 0, text_width + 16 + offset, PARAMETER_HEIGHT))
        @sprite.bitmap.draw_text(offset, 0, @sprite.bitmap.width - offset, @sprite.bitmap.height, name, 1)
        #if @icon_position
        #  @sprite.bitmap.blt((PARAMETER_WIDTH - text_width - offset - 16) / 2, (PARAMETER_HEIGHT - ICON_SIZE * 2) / 2, RPG::Cache.menu("icons"), Rect.new(@icon_position[0] * ICON_SIZE, @icon_position[1] * ICON_SIZE, ICON_SIZE, ICON_SIZE))
        #end
      end
    end
  end

  # ----------------------------------------------------------------------------------------
  class BoolParameter < BaseParameter
    TYPE = :bool

    def initialize(settings_content, screen_id, position, name, icon, parameter, bool_value)
      super(settings_content, screen_id, position, name, icon, parameter, bool_value || false)
      @value_width = BOOL_VALUE_WIDTH
    end

    def value=(value)
      if (value != self.value)
        Audio.se_play(PARAMETER_CHANGE_AUDIO, 70, value ? 125 : 75)
      end
      super(value)
    end

    def get_display_value
      return self.value ? "ON" : "OFF"
    end
    
    def value_left()
      return if @disabled
      self.value = !self.value
      redraw
    end
    def value_right()
      return if @disabled
      self.value = !self.value
      redraw
    end
    def action()
      return if @disabled
      self.value = !self.value
      redraw
    end
  end

  # ----------------------------------------------------------------------------------------
  class SwitchPatameter < BoolParameter
    TYPE = :switch

    def initialize(settings_content, screen_id, position, name, icon, parameter, bool_value, switch, invert)
      @switch = switch
      @invert = invert

      super(settings_content, screen_id, position, name, icon, parameter, bool_value)
    end
    
    def value=(value)
      super()

      if $game_switches[@switch]
        $game_switches[@switch] = @invert ? !@value : @value
      end
    end
  end

  # ----------------------------------------------------------------------------------------
  class IntParameter < BaseParameter
    TYPE = :int
    
    attr_reader :max_value
    attr_reader :min_value

    def initialize(settings_content, screen_id, position, name, icon, parameter, int_value, min_value, max_value)
      @min_value = min_value
      @max_value = max_value

      super(settings_content, screen_id, position, name, icon, parameter, int_value)
    end

    def value=(value)
      if (value != self.value)
        Audio.se_play(PARAMETER_CHANGE_AUDIO, 70, ((value.to_f + @min_value.to_f) / (@max_value.to_f + @min_value.to_f) * 50.0).to_i + 75)
      end
      super(value)
    end

    def value_left()
      return if @disabled
      self.value = (self.value - 1).clamp(@min_value, @max_value)
      redraw
    end
    def value_right()
      return if @disabled
      self.value = (self.value + 1).clamp(@min_value, @max_value)
      redraw
    end
  end

  # ----------------------------------------------------------------------------------------
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
      redraw_icon
      offset = !!@icon_position ? ICON_SIZE * 2 + 8 : 0
      @sprite.bitmap.clear
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

  # ----------------------------------------------------------------------------------------
  class FloatParameter < BaseParameter
    TYPE = :float
    
    attr_reader :max_value
    attr_reader :min_value
    attr_reader :step

    def initialize(settings_content, screen_id, position, name, icon, parameter, float_value, step, min_value, max_value)
      @step = step
      @min_value = min_value
      @max_value = max_value

      super(settings_content, screen_id, position, name, icon, parameter, float_value)
    end

    def value=(value)
      if (value != @value)
        Audio.se_play(PARAMETER_CHANGE_AUDIO, 70, ((value + @min_value) / (@max_value + @min_value) * 50.0).to_i + 75)
      end
      super(value)
    end

    def value_left()
      return if @disabled
      self.value = (self.value - @step).clamp(@min_value, @max_value)
      redraw
    end
    def value_right()
      return if @disabled
      self.value = (self.value + @step).clamp(@min_value, @max_value)
      redraw
    end
  end

  # ----------------------------------------------------------------------------------------
  class EnumParameter < IntParameter
    TYPE = :enum

    def initialize(settings_content, screen_id, position, name, icon, parameter, int_value, enum_texts)
      @texts = enum_texts

      super(settings_content, screen_id, position, name, icon, parameter, int_value, 0, enum_texts.length - 1)
    end

    def get_display_value
      @texts&.[](self.value) || "#{tr("Unknown value")} #{self.value}"
    end

    def value_left()
      return if @disabled
      self.value = (self.value - 1) % [1, @max_value + 1].max
      redraw
    end
    def value_right()
      return if @disabled
      self.value = (self.value + 1) % [1, @max_value + 1].max
      redraw
    end
  end
  
  #----------------------------------------------------------------------------------------
  class KeyParameter < BaseParameter
    TYPE = :key

    SELECTED_KEYS_MARGIN = 20

    attr_reader :selection

    def initialize(settings_content, screen_id, position, name, icon, parameter, key_binds, input_bind)
      @selected = false
      @selection = 0
      @key_binds = parameter ? Settings[parameter] : key_binds
      @bind = input_bind
      @waiting_for_key = false
      @accept_action = false

      super(settings_content, screen_id, position, name, icon, parameter, nil)

      apply
    end

    def get_display_value(bind_id = 0)
      key_bind = @key_binds[bind_id]
      
      if key_bind == nil
        return "None"
      end
      
      case key_bind.type

      when KeyBind::Type::Invalid
        return "Invalid"

      when KeyBind::Type::Key
        return Input::key_name(key_bind.scancode)

      when KeyBind::Type::CButton
        return Input::c_button_name(key_bind.button)
      
      when KeyBind::Type::CAxis
        axis_name = Input::c_axis_name(key_bind.axis)
        dir_vert = axis_name.downcase.include?("y")
        dir_horiz = axis_name.downcase.include?("x")
        return axis_name +
                  if dir_horiz || dir_vert 
                    " " + 
                    if dir_vert
                      (key_bind.dir == 1 ? "Up" : "Down")
                    else
                      (key_bind.dir == 1 ? "Right" : "Left")
                    end
                  else "" end
      
      when KeyBind::Type::JButton
        return tr("Joystick Button") + " " + key_bind.button.to_s
        
      when KeyBind::Type::JAxis
        return tr("Joystick Axis") + " " + key_bind.axis.to_s + " " + key_bind.dir == 1 ? tr("Positive") : tr("Negative")
        
      when KeyBind::Type::JHat
        return tr("Joystick Hat") + " " + key_bind.hat.to_s + " " + key_bind.pos == 1 ? tr("Positive") : tr("Negative")

      end
    end
    
    def redraw()
      redraw_icon
      offset = !!@icon_position ? ICON_SIZE * 2 + 8 : 0
      if (@parameter)
        @key_binds = Settings[@parameter]
      end

      @sprite.bitmap.clear
      
      if @selected
        @sprite.bitmap.fill_rect(Rect.new(@sprite.bitmap.width - PARAMETER_KEY_WIDTH * (4 - @selection), 0, PARAMETER_KEY_WIDTH, @sprite.bitmap.height), Color.new(255, 255, 255, 48))
        @sprite.bitmap.draw_text(@sprite.bitmap.width - PARAMETER_KEY_WIDTH * (4 - @selection), 0, SELECTED_KEYS_MARGIN, @sprite.bitmap.height, "→", 1)
        @sprite.bitmap.draw_text(@sprite.bitmap.width - PARAMETER_KEY_WIDTH * (3 - @selection) - SELECTED_KEYS_MARGIN, 0, SELECTED_KEYS_MARGIN, @sprite.bitmap.height, "←", 1)
      end
      
      @sprite.bitmap.draw_text(offset, 0, @sprite.bitmap.width - PARAMETER_KEY_WIDTH * 4 - offset, @sprite.bitmap.height, tr(@name))
      for i in 0..3
        offset = (@selected && i == @selection ? SELECTED_KEYS_MARGIN : 4)
        parameter_x = @sprite.bitmap.width - PARAMETER_KEY_WIDTH * (4 - i) + offset
        @sprite.bitmap.draw_text(parameter_x, 0, PARAMETER_KEY_WIDTH - offset * 2, @sprite.bitmap.height, @waiting_for_key && i == @selection ? tr("Press a key") : tr(get_display_value(i)), 1)
        @sprite.bitmap.fill_rect(Rect.new(@sprite.bitmap.width - PARAMETER_KEY_WIDTH * (4 - i) - 1, 1, 2, @sprite.bitmap.height - 2), Color.new(255, 255, 255, 32))
      end

      if (@settings_content.need_draw_line(@screen_id, @position))
        @sprite.bitmap.fill_rect(Rect.new(0, PARAMETER_HEIGHT - 1, PARAMETER_WIDTH, 2), Color.new(255, 255, 255, 24))
      end

      #if @icon_position
      #  @sprite.bitmap.blt(0, (PARAMETER_HEIGHT - ICON_SIZE * 2) / 2, RPG::Cache.menu("icons"), Rect.new(@icon_position[0] * ICON_SIZE, @icon_position[1] * ICON_SIZE, ICON_SIZE, ICON_SIZE))
      #end
    end

    def value_left()
      if !@waiting_for_key
        @selection = (@selection - 1) % 4
        $game_system.se_play($data_system.cursor_se)
        redraw
      end
    end
    def value_right()
      if !@waiting_for_key
        @selection = (@selection + 1) % 4
        $game_system.se_play($data_system.cursor_se)
        redraw
      end
    end

    def action()
      return if @disabled
      @accept_action = false
      @waiting_for_key = true
      @settings_content.waiting_for_key = true
      redraw
    end

    def apply()
      if @bind != nil
        Input::set_binding(@key_binds, @bind)
      end
    end

    def update()
      super()

      if @waiting_for_key && @settings_content.waiting_for_key
        key = Input::pressed_key
        c_button = Input::pressed_c_button
        c_axis = Input::active_c_axis

        if !Input.press?(Input::ACTION)
          @accept_action = true
        end

        if key && (!Input.press?(Input::ACTION) || @accept_action)
          if key != Input::key_from_name("Backspace")
            Settings[@parameter][@selection] = KeyBind.key(key)
          else
            Settings[@parameter][@selection] = nil
          end
          @waiting_for_key = @settings_content.waiting_for_key = false
          apply
          redraw
        elsif c_button
          Settings[@parameter][@selection] = KeyBind.cbutton(c_button)
          @waiting_for_key = @settings_content.waiting_for_key = false
          apply
          redraw
        elsif c_axis
          Settings[@parameter][@selection] = KeyBind.caxis(c_axis, Input::c_axis_pressure(c_axis) > 0 ? KeyBind::Positive : KeyBind::Negative)
          @waiting_for_key = @settings_content.waiting_for_key = false
          apply
          redraw
        end
      end
    end

    def select(previous)
      prev = @settings_content.get_parameter_by_sceen_id(@settings_content.screen, previous)
      if prev&.respond_to?(:selection)
        @selection = prev.selection
      end

      @selected = true
      redraw
    end
    def deselect
      @selected = false
      redraw
    end
  end
  class ActionParameter < BaseParameter
    TYPE = :action

    def initialize(settings_content, screen_id, position, name, icon, value_text, func, arg1, arg2, arg3, arg4)
      @func = func
      @arg1 = arg1
      @arg2 = arg2
      @arg3 = arg3
      @arg4 = arg4

      super(settings_content, screen_id, position, name, icon, nil, value_text)
    end
    
    def action()
      return if @disabled
      if @arg1 == nil && @arg2 == nil && @arg3 == nil && @arg4 == nil
        @func.call
      elsif @arg2 == nil && @arg3 == nil && @arg4 == nil
        @func.call(@arg1)
      elsif @arg3 == nil && @arg4 == nil
        @func.call(@arg1, @arg2)
      elsif @arg4 == nil
        @func.call(@arg1, @arg2, @arg3)
      else
        @func.call(@arg1, @arg2, @arg3, @arg4)
      end
      @settings_content.redraw_all
      $game_system.se_play($data_system.buzzer_se)
    end
  end
end