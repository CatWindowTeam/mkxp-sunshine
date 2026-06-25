class Window_Settings
  MARGIN = 15
  TITLE_TOP_MARGIN = 16
  TITLE_MARGIN = 40
  ITEM_SPACING = 20
  VALUE_MARGIN = 270
  ACTIVE_MARGIN = MARGIN * 2 + 20

  def initialize
    @viewport = Viewport.new(0, 0, Graphics.width, Graphics.height)
    @viewport.z = 9998
    @bg = Sprite.new(@viewport)
    @bg.bitmap = Bitmap.new(Graphics.width, Graphics.height)
    @bg.bitmap.fill_rect(0, 0, Graphics.width, Graphics.height, Color.new(255, 255, 255, 196))
    @bg.blend_type = 2
    @title = Sprite.new(@viewport)
    @title.bitmap = Bitmap.new(320, TITLE_MARGIN)
    @title.bitmap.font.size = 20
    @title.zoom_x = @title.zoom_y = 2
    @title.y = TITLE_TOP_MARGIN
    @title.x = MARGIN
    @title.bitmap.draw_text(0, 0, @title.bitmap.width, @title.bitmap.height, tr("Settings"))
    Language.register_text_sprite(self.class.name + "_title", @title)

    @content = SettingsContent.new(@viewport, TITLE_TOP_MARGIN + TITLE_MARGIN + 100)

    DATA.each_with_index do |(screen_title, parameters_info), screen_index|
      @content.add_screen(screen_title)
      parameters_info.each_with_index do |parameter_info, parameter_index|
        if parameters_info == :sep
          @content.add_parameter(screen_title, Separator.new(@content, screen_index, parameter_index, ""))
        else
          case parameter_info[:type]
          when :base
            @content.add_parameter(screen_title, BaseParameter.new(@content, screen_index, parameter_index,
              parameter_info[:name], parameter_info[:parameter], parameter_info[:default]))
          when :bool
            @content.add_parameter(screen_title, BoolParameter.new(@content, screen_index, parameter_index,
              parameter_info[:name], parameter_info[:parameter], parameter_info[:default]))
          when :switch
            @content.add_parameter(screen_title, BoolParameter.new(@content, screen_index, parameter_index,
              parameter_info[:name], parameter_info[:parameter], parameter_info[:default], parameter_info[:switch], parameter_info[:invert]))
          when :int
            @content.add_parameter(screen_title, IntParameter.new(@content, screen_index, parameter_index,
              parameter_info[:name], parameter_info[:parameter], parameter_info[:default], parameter_info[:min], parameter_info[:max]))
          when :slider
            @content.add_parameter(screen_title, SliderParameter.new(@content, screen_index, parameter_index,
              parameter_info[:name], parameter_info[:parameter], parameter_info[:default], parameter_info[:min], parameter_info[:max]))
          when :float
            @content.add_parameter(screen_title, FloatParameter.new(@content, screen_index, parameter_index,
              parameter_info[:name], parameter_info[:parameter], parameter_info[:default], parameter_info[:step], parameter_info[:min], parameter_info[:max]))
          when :enum
            @content.add_parameter(screen_title, EnumParameter.new(@content, screen_index, parameter_index,
              parameter_info[:name], parameter_info[:parameter], 0, parameter_info[:values]))
          when :sep
            @content.add_parameter(screen_title, Separator.new(@content, screen_index, parameter_index,
              parameter_info[:name]))
          when :key
            @content.add_parameter(screen_title, KeyParameter.new(@content, screen_index, parameter_index,
              parameter_info[:name], parameter_info[:parameter], parameter_info[:key_binds], parameter_info[:bind]))
          end
        end
      end
    end
    @content.redraw_all

	  @left_hold_timer = 0
	  @right_hold_timer = 0

	  @up_hold_timer = 0
	  @down_hold_timer = 0

    @visible = self.visible = false
    RPG::Mod.exec_hooks("hooks/Window_Settings/init", binding)
  end

  def dispose
    @bg.dispose
    @title.dispose
    @content.dispose
  end

  def open
    self.visible = true
    self.opacity = 0
    @fade_in = true
  end

  def redraw_setting(screen, index)
    if @visible == false
      return
    end
    
    @content.get_parameter_by_sceen_id(screen, index)&.redraw
  end

  def self.load_settings
    Settings.load!
  end

  def update
    if self.visible
      @content.update
    end

    if @fade_in
      self.opacity += 20
      if self.opacity == 255
        @fade_in = false
      end
      return
    end

    if @fade_out
      self.opacity -= 20
      if self.opacity == 0
        @fade_out = false
        self.visible = false
      end
      return
    end

    return if !self.visible
    return if @content.waiting_for_key

    # navigation
    if Input.wheel_y != 0
      if (Input::key_press?(Input::key_from_name("Left Shift")))
        for i in 0...Input.wheel_y.round.abs
          Input.wheel_y.round > 0 ? @content.parameter_right : @content.parameter_left
        end
      else
        @content.parameter_select_offset(-Input.wheel_y.round)
        $game_system.se_play($data_system.cursor_se)
      end
    end

    if (Input.trigger?(Input::L))
      @content.screen_left
      $game_system.se_play($data_system.cursor_se)
    end
    if (Input.trigger?(Input::R))
      @content.screen_right
      $game_system.se_play($data_system.cursor_se)
    end

    # hold timers
  	if Input.press?(Input::LEFT)
  	  @left_hold_timer += 1
  	else
  	  @left_hold_timer = 0
  	end

  	if Input.press?(Input::RIGHT)
  	  @right_hold_timer += 1
  	else
  	  @right_hold_timer = 0
  	end

  	if Input.press?(Input::UP)
  	  @up_hold_timer += 1
  	else
  	  @up_hold_timer = 0
  	end

  	if Input.press?(Input::DOWN)
  	  @down_hold_timer += 1
  	else
  	  @down_hold_timer = 0
  	end

    #parameters navigation
    if Input.trigger?(Input::UP) || (Input.press?(Input::UP) && (@up_hold_timer >= 15))
      @up_hold_timer -= 2
      @content.parameter_up
      $game_system.se_play($data_system.cursor_se)
    elsif Input.trigger?(Input::DOWN) || (Input.press?(Input::DOWN) && (@down_hold_timer >= 15))
      @down_hold_timer -= 2
      @content.parameter_down
      $game_system.se_play($data_system.cursor_se)
    end

    # parameter setting
    current_parameter = @content.get_current_parameter
    old_val = current_parameter&.value
    if Input.trigger?(Input::LEFT) || (Input.press?(Input::LEFT) && (@left_hold_timer >= 15))
      @left_hold_timer -= 2
      @content.parameter_left
    elsif Input.trigger?(Input::RIGHT) || (Input.press?(Input::RIGHT) && (@right_hold_timer >= 15))
      @right_hold_timer -= 2
      @content.parameter_right
    end
    if (Input.trigger?(Input::ACTION))
      current_parameter&.action
    end
    
    # menu closing
    if Input.trigger?(Input::CANCEL)
      $game_system.se_play($data_system.cancel_se)
      @fade_out = true

      Settings.save!
    end
  end

  # Attributes
  def visible
    return @visible
  end
  def visible=(val)
    @viewport.visible = val
    @visible = val
  end
  def opacity=(val)
    @bg.opacity = val
    @content.opacity = val
  end
  def opacity
    @bg.opacity
  end
end
