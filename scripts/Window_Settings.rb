class Window_Settings
  MARGIN = 15
  TITLE_TOP_MARGIN = 16
  TITLE_MARGIN = 40
  ITEM_SPACING = 20
  VALUE_MARGIN = 270
  ACTIVE_MARGIN = MARGIN * 2 + 20

  def initialize
    @viewport = Viewport.new(0, 0, Graphics.width, Graphics.height)
    @bg = Sprite.new(@viewport)
    @bg.bitmap = Bitmap.new(Graphics.width, Graphics.height)
    @bg.bitmap.fill_rect(0, 0, Graphics.width, Graphics.height, Color.new(0, 0, 0, 128))
    @title = Sprite.new(@viewport)
    @title.bitmap = Bitmap.new(320, TITLE_MARGIN)
    @title.bitmap.font.size = 40
    @title.y = TITLE_TOP_MARGIN
    @title.x = MARGIN
    @version = Sprite.new(@viewport)
    @version.bitmap = Bitmap.new(60, 20)
    @version.x = 10
    @version.y = Graphics.width / 1.9
    @version.opacity = 128
    Language.register_text_sprite(self.class.name + "_title", @title)
    Language.register_text_sprite(self.class.name + "_ver", @version)
    @data_sprites = []
    @viewport.z = 9998

    @left_hold_timer = 0
    @right_hold_timer = 0

    self.visible = false
    @index = 0
    @fade_in = false
    @fade_out = false

    @transfer_player = nil
	@visible = false
	RPG::Mod.exec_hooks("hooks/Window_Settings/init", binding)
  end

  def dispose
    @bg.dispose
    @title.dispose
    @version.dispose
    @data_sprites.each do |spr|
      spr.dispose
    end
    @viewport.dispose
  end

  def open
    self.visible = true
    self.opacity = 0
    @fade_in = true
    @data = [
	         tr('BGM Volume'),
			 tr('SFX Volume'),
			 tr('Fullscreen'),
			 tr('Default movement'),
			 tr('Colorblind mode'),
             tr('Configure controls(Press F1)'),
			 tr('Skip Text (R)'),
			 tr('Frameskip'),
             tr('In-Game Timer'),
			 tr('Language'),
             tr('Debug mode(!)'),
             tr('Freeware Mode(!)'),
			]

    @index = 0
    # Load our language settings from persistent, stored differently
    @lang_index = Language::LANGUAGES.index($persistent.langcode)
    if @lang_index.nil?
        @lang_index = 0
    end

    # Create title
    @title.bitmap.clear
    @title.bitmap.draw_text(0, 0, @title.bitmap.width, @title.bitmap.height, tr("Settings"))

    # Create version string
    @version.bitmap.clear
    @version.bitmap.draw_text(0, 0, @version.bitmap.width, @version.bitmap.height, tr("POT_VERSION"))

    # Create menu options
    @data.each_with_index do |item, i|
      spr = Sprite.new(@viewport)
      spr.bitmap = Bitmap.new(400, ITEM_SPACING)
      spr.x = MARGIN
      spr.y = TITLE_MARGIN + TITLE_TOP_MARGIN + ITEM_SPACING * i
      spr.opacity = 0
      Language.register_text_sprite(self.class.name + "_option_#{i}", spr)
	    redraw_setting(spr, i)

      @data_sprites << spr
    end
  end

  def redraw_setting(spr, i)
      if @visible == false
	    return
	  end
      spr.bitmap.clear
      spr.bitmap.draw_text(0, 0, spr.bitmap.width, spr.bitmap.height, @data[i])
      spr.bitmap.draw_text(0, 0, spr.bitmap.width, spr.bitmap.height, @data[i])
	  case i
	    when 0
          spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, Audio.bgm_volume.to_s)
		when 1
          spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, Audio.sfx_volume.to_s)
		when 2 #fullscreen
		  if(Graphics.fullscreen == true)
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("ON"))
		  else
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("OFF"))
		  end
		when 3 #default movement
		  if($game_switches[251] == true)
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("RUN"))
		  else
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("WALK"))
		  end
		when 4 #colorblind mode
		  if($game_switches[252] == true)
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("ON"))
		  else
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("OFF"))
		  end
		when 6 #automash
		  if($game_switches[253] == true)
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("ON"))
		  else
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("OFF"))
		  end
		when 7 #frameskip
		  if(Graphics.frameskip == true)
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("ON"))
		  else
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("OFF"))
		  end
        when 8 # In-game timer
          text = tr($game_temp.igt_timer_visible ? "ON" : "OFF")
          spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, text)
        when 9 # Language
          l = Language::LANGUAGES[@lang_index] rescue Language::LANGUAGES[0]
          spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr(l))
        when 10
          if(Settings[:debug] == true)
            spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("ON"))
          else
            spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("OFF"))
          end
        when 11
          if(Settings[:oneshot_mode] == true)
            spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("ON"))
          else
            spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("OFF"))
          end
        end
  end


  def redraw_setting_index(i)
    if !@data_sprites.kind_of?(Array)
	  return
	end
	if @data_sprites.length - 1 < i
	  return
	end
    redraw_setting(@data_sprites[i], i)
  end


  def redraw_all_settings
    @title.bitmap.clear
    @title.bitmap.draw_text(0, 0, @title.bitmap.width, @title.bitmap.height, tr("Settings"))
    @version.bitmap.clear
    @version.bitmap.draw_text(0, 0, @version.bitmap.width, @version.bitmap.height, tr("POT_VERSION"))
    for i in 0..(@data_sprites.length - 1)
      redraw_setting_index(i)
    end
  end

  def self.load_settings
    Settings.load!
  end

  def update
    if @visible
      @version.bitmap.clear
      @version.bitmap.draw_text(0, 0, @version.bitmap.width, @version.bitmap.height, Input.wheel_y.to_s)
    end
    
	@version.opacity = 128
  
    if @fade_in
      self.opacity += 20
      active_spr = nil
      @data_sprites.each do |spr|
        spr.opacity += 10
        spr.opacity = 128 if spr.opacity > 128
        active_spr = spr if !active_spr && spr.x < MARGIN * 2
      end
      if active_spr
        active_spr.x += 6
        active_spr.x = MARGIN * 2 if active_spr.x > MARGIN * 2
      elsif self.opacity == 255
        @fade_in = false
      end
	  @version.opacity = 128
      return
    end

    if @fade_out
      self.opacity -= 20
      @data_sprites.each do |spr|
        spr.opacity -= 10
      end
      if self.opacity == 0
        @fade_out = false
        self.visible = false
        @data_sprites.each do |spr|
          spr.dispose
        end
        @data_sprites = []

        if @transfer_player
          $game_temp.player_transferring = true
          $game_temp.player_new_map_id = @transfer_player.id
          $game_temp.player_new_x = @transfer_player.x
          $game_temp.player_new_y = @transfer_player.y
          $game_temp.player_new_direction = @transfer_player.dir
          Graphics.freeze
          $game_temp.transition_processing = true
          $game_temp.transition_name = "black"
          @transfer_player = nil
        end
      end
      return
    end

    return if !self.visible

    # Adjust position and visibility
    @data_sprites.each_with_index do |spr, i|
      if i == @index
        if spr.x < ACTIVE_MARGIN
          spr.x += 6
          spr.x = ACTIVE_MARGIN if spr.x > ACTIVE_MARGIN
        end
        spr.opacity += 10 if spr.opacity < 255
      else
        if spr.x > MARGIN * 2
          spr.x -= 6
          spr.x = MARGIN * 2 if spr.x < MARGIN * 2
        end
        spr.opacity -= 10 if spr.opacity > 128
        spr.opacity = 128 if spr.opacity < 128
      end
    end

    if Input.wheel_y != 0
      @index = (@index - Input.wheel_y.round) % @data.size
      $game_system.se_play($data_system.cursor_se)
    end

    if Input.trigger?(Input::UP)
      @index = (@index - 1) % @data.size
      $game_system.se_play($data_system.cursor_se)
    end
    if Input.trigger?(Input::DOWN)
      @index = (@index + 1) % @data.size
      $game_system.se_play($data_system.cursor_se)
    end

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

	case @index
	  when 0 #bgm vol
		old_vol = Audio.bgm_volume
	    if Input.trigger?(Input::LEFT) || (Input.press?(Input::LEFT) && (@left_hold_timer >= 15))
		  @left_hold_timer -= 2
		  Audio.bgm_volume -= 1
		  if old_vol > 1
            Audio.se_play("Audio/SE/text_robot.wav", 70, (Audio.bgm_volume/2) + 75)
		    redraw_all_settings()
		  end
		elsif Input.trigger?(Input::RIGHT) || (Input.press?(Input::RIGHT) && (@right_hold_timer >= 15))
		  @right_hold_timer -= 2
		  Audio.bgm_volume += 1
		  if old_vol < 100
            Audio.se_play("Audio/SE/text_robot.wav", 70, (Audio.bgm_volume/2) + 75)
		    redraw_all_settings()
		  end
		end

	  when 1 #sfx vol
		old_vol = Audio.sfx_volume
	    if Input.trigger?(Input::LEFT) || (Input.press?(Input::LEFT) && (@left_hold_timer >= 15))
		  @left_hold_timer -= 2
		  Audio.sfx_volume -= 1
		  if old_vol > 1
            Audio.se_play("Audio/SE/text_robot.wav", 70, (Audio.sfx_volume/2) + 75)
		    redraw_all_settings()
		  end
		elsif Input.trigger?(Input::RIGHT) || (Input.press?(Input::RIGHT) && (@right_hold_timer >= 15))
		  @right_hold_timer -= 2
		  Audio.sfx_volume += 1
		  if old_vol < 100
            Audio.se_play("Audio/SE/text_robot.wav", 70, (Audio.sfx_volume/2) + 75)
		    redraw_all_settings()
		  end
		end

	  when 2 #fullscreen
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)

		  $game_system.se_play($data_system.decision_se)
          if Graphics.fullscreen == true
	        Graphics.fullscreen = false
		    $console = false
	      else
	        Graphics.fullscreen = true
		    $console = true
	      end
	      sleep(0.500)
		  redraw_all_settings()
	      sleep(0.500)
		end

	  when 3 #default movement
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)

		  $game_system.se_play($data_system.decision_se)
          if $game_switches[251] == true
	        $game_switches[251] = false
	      else
	        $game_switches[251] = true
	      end
		  redraw_all_settings()
		end

	  when 4 #colorblind mode
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)

		  $game_system.se_play($data_system.decision_se)
          if $game_switches[252] == true
	        $game_switches[252] = false
	      else
	        $game_switches[252] = true
	      end
		  redraw_all_settings()
		end

	  when 6 #automash
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)

		  $game_system.se_play($data_system.decision_se)
          if $game_switches[253] == true
	        $game_switches[253] = false
	      else
	        $game_switches[253] = true
	      end
		  redraw_all_settings()
		end

	  when 7 #frameskip
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)

		  $game_system.se_play($data_system.decision_se)
          if Graphics.frameskip == true
	        Graphics.frameskip = false
	      else
	        Graphics.frameskip = true
	      end
		  redraw_all_settings()
		end
      when 8 # In-game timer
        if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)
          $game_system.se_play($data_system.decision_se)
          $game_temp.igt_timer_visible = !$game_temp.igt_timer_visible
          $scene.in_game_timer.visible = $game_temp.igt_timer_visible if $scene.is_a?(Scene_Map)
          redraw_all_settings()
        end
	  when 9 #language
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::RIGHT)
          $game_system.se_play($data_system.decision_se)
          @lang_index += 1
          if @lang_index >= Language::LANGUAGES.length
            @lang_index = 0
          end
          $persistent.lang = Language::LANGUAGES[@lang_index]
          redraw_all_settings()
		end
		if Input.trigger?(Input::LEFT)
          $game_system.se_play($data_system.decision_se)
		  @lang_index -= 1
		  if @lang_index < 0
		    @lang_index = Language::LANGUAGES.length - 1
		  end
          $persistent.lang = Language::LANGUAGES[@lang_index]
          redraw_all_settings()
		end
      when 10
        if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)
          $game_system.se_play($data_system.decision_se)
          Settings[:debug] = !Settings[:debug]
          redraw_all_settings()
        end
      when 11
        if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)
          $game_system.se_play($data_system.decision_se)
          Settings[:oneshot_mode] = !Settings[:oneshot_mode]
          redraw_all_settings()
        end
	end

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
    @title.opacity = val
    @version.opacity = val
  end
  def opacity
    @bg.opacity
  end
end
