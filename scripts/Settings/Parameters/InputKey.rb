class Window_Settings
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
      if @parameter && @key_binds != Settings[@parameter]
        @key_binds = Settings[@parameter]
        apply
      end

      @sprite.bitmap.clear
      
      if @selected
        @sprite.bitmap.fill_rect(Rect.new(@sprite.bitmap.width - PARAMETER_KEY_WIDTH * (4 - @selection), 0, PARAMETER_KEY_WIDTH, @sprite.bitmap.height), Color.new(255, 255, 255, 48))
        @sprite.bitmap.draw_text(@sprite.bitmap.width - PARAMETER_KEY_WIDTH * (4 - @selection), 0, SELECTED_KEYS_MARGIN, @sprite.bitmap.height, "→", 1)
        @sprite.bitmap.draw_text(@sprite.bitmap.width - PARAMETER_KEY_WIDTH * (3 - @selection) - SELECTED_KEYS_MARGIN, 0, SELECTED_KEYS_MARGIN, @sprite.bitmap.height, "←", 1)
      end
      
      @sprite.bitmap.draw_text(offset, 0, @sprite.bitmap.width - PARAMETER_KEY_WIDTH * 4 - offset, @sprite.bitmap.height, tr(@name))
      (0..3).each do |i|
        offset = (@selected && i == @selection ? SELECTED_KEYS_MARGIN : 4)
        parameter_x = @sprite.bitmap.width - PARAMETER_KEY_WIDTH * (4 - i)
        key_bind = @key_binds[i]
        if !(@waiting_for_key && i == @selection) && key_bind && key_bind.type > KeyBind::Type::Key && key_bind.type < KeyBind::Type::JButton
          icon_x, icon_y = case key_bind.type
          when KeyBind::Type::CButton
            GamepadIcons.button(key_bind.button)
          when KeyBind::Type::CAxis
            GamepadIcons.axis(key_bind.axis, key_bind.dir)
          end

          @sprite.bitmap.stretch_blt(Rect.new(parameter_x + PARAMETER_KEY_WIDTH / 2 - ICON_SIZE, @sprite.bitmap.height / 2 - ICON_SIZE, ICON_SIZE * 2, ICON_SIZE * 2), RPG::Cache.menu("gamepad_icons"), Rect.new(icon_x * ICON_SIZE, icon_y * ICON_SIZE, ICON_SIZE, ICON_SIZE))
        else
          @sprite.bitmap.draw_text(parameter_x + offset, 0, PARAMETER_KEY_WIDTH - offset * 2, @sprite.bitmap.height, @waiting_for_key && i == @selection ? tr("Press a key") : tr(get_display_value(i)), 1)
        end
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
        elsif c_button && (!Input.press?(Input::ACTION) || @accept_action)
          Settings[@parameter][@selection] = KeyBind.cbutton(c_button)
          @waiting_for_key = @settings_content.waiting_for_key = false
          apply
          redraw
        elsif c_axis && (!Input.press?(Input::ACTION) || @accept_action)
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
end