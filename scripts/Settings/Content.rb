class Window_Settings
  SCREENS_PANELS_MARGIN = 32
  SCREENS_PANELS_TOP_MARGIN = 16
  PARAMETER_WIDTH = (Graphics.width - 640) / 2 + 512
  PARAMETER_HEIGHT = 32
  PARAMETER_VALUE_WIDTH = PARAMETER_WIDTH / 2
  PARAMETER_KEY_WIDTH = PARAMETER_WIDTH / 5
  ICON_SIZE = 16
  ICON_SCALE = 2
  ICON_SCALED = ICON_SIZE * ICON_SCALE

  PARAMETER_CHANGE_AUDIO = "Audio/SE/text_robot.wav"

  class SettingsContent
    attr_reader :x
    attr_reader :screen
    attr_reader :index
    attr_reader :viewport
    attr_reader :offset
    attr_accessor :icons_atlas

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
      @selection_sprite.bitmap = Bitmap.new(1, 1)
      @selection_sprite.bitmap.fill_rect(Rect.new(0, 0, 1, 1), Color.new(255, 255, 255, 64))
      @selection_sprite.zoom_x = PARAMETER_WIDTH + 16
      @selection_sprite.zoom_y = PARAMETER_HEIGHT
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

      @icons_atlas = RPG::Cache.menu("icons")

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
    def get_parameter_by_setting(setting)
      @parameters.each do |screen_name, screen_parameters|
        screen_parameters.each do |parameter|
          if parameter.parameter == setting
            return parameter
          end
        end
      end
      return nil
    end

    def disable_setting(screen, position)
      if screen.is_a?(String)
        get_parameter(screen, position).disabled = true
      else
        get_parameter_by_sceen_id(screen, position).disabled = true
      end
    end
    def enable_setting(screen, position)
      if screen.is_a?(String)
        get_parameter(screen, position).disabled = false
      else
        get_parameter_by_sceen_id(screen, position).disabled = false
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

      # selection sprite
      selection_sprite_result_pos = @index * PARAMETER_HEIGHT + @y
      if (@selection_sprite.y - selection_sprite_result_pos).abs <= 1
        @selection_sprite.y = selection_sprite_result_pos
      end
      @selection_sprite.y = selection_sprite_result_pos * 0.5 + @selection_sprite.y * 0.5
      @selection_sprite.x = get_current_parameter.x * 0.5 + @selection_sprite.x * 0.5 - 4
      @selection_sprite.zoom_x = get_current_parameter.width * 0.5 + @selection_sprite.zoom_x * 0.5 + 8

      # update parameters
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
        @index = (@index + (offset <=> 0)) % @parameters.values[@screen].length
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
    end

    def waiting_for_key=(value)
      @waiting_for_key = value
    end
    def waiting_for_key
      @wait_timer > 0
    end
  end
end