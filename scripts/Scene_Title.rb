#==============================================================================
# ** Scene_Title
#------------------------------------------------------------------------------
#  This class performs title screen processing.
#==============================================================================

class Scene_Title
  MENU_X = 150
  MENU_Y = 100
  ENTRY_HEIGHT = 25
  SDLVer = "#{Sunshine::SDLVersion_major}.#{Sunshine::SDLVersion_minor}.#{Sunshine::SDLVersion_micro}"
  SunshineVer = "0.1.1-dev"
  
  #--------------------------------------------------------------------------
  # * Main Processing
  #--------------------------------------------------------------------------
  def main
    # Load database
    $data_actors        = load_data("Data/Actors.rxdata")
    $data_items         = load_data("Data/Items.rxdata")
    $data_armors        = load_data("Data/Armors.rxdata")
    $data_animations    = load_data("Data/Animations.rxdata")
    $data_tilesets      = load_data("Data/Tilesets.rxdata")
    $data_common_events = load_data("Data/CommonEvents.rxdata")
    $data_system        = load_data("Data/System.rxdata")

    Language.initialize_database

    $game_temp = Game_Temp.new
    new_game
    $game_system = Game_System.new
	
    Settings.load! # loading settings

    load_perma_flags
    Oneshot.allow_exit true
	
    @window_settings_title = Window_Settings.new

    @viewport = Viewport.new(0, 0, Graphics.width, Graphics.height)

    # Make title graphic
    @sprite = Sprite.new(@viewport)
	
    # chinese has its own special title screen so check for it
    translation_name = "#{$persistent.langcode}/#{$data_system.title_name}"
    if File.exist?("Graphics/Titles/#{translation_name}.png")
      @sprite.bitmap = RPG::Cache.title(translation_name)
    else
      if File.exist?("badend.lock")
        @sprite.bitmap = RPG::Cache.title("badend")
      else
        @sprite.bitmap = RPG::Cache.title($data_system.title_name)
      end
    end
    @sprite.x = Graphics.width / 2
    @sprite.y = Graphics.height / 2
    px = (Graphics.width / 1920.0)
    py = (Graphics.height / 1080.0)
    @sprite.ox = @sprite.bitmap.width / 2 * (0.5 * (1.0 - px) + px)
    @sprite.oy = @sprite.bitmap.height / 2 * (1.3 * (1.0 - py) + py)

    RPG::Mod.exec_hooks("hooks/Scene_Title/init", binding)

    # check for debug file to add debug items
    if Settings[:debug]
      $game_party.gain_item(54, 1) # debug save
        $game_party.gain_item(82, 1) # plight skip
      $game_party.gain_item(81, 1) # George reroler
    end 

    @sprite.zoom_x = 2.0
    @sprite.zoom_y = 2.0
    # Create/render menu options
    @menu = Sprite.new(@viewport)
    @menu.z += 1
    @menu.bitmap = Bitmap.new(MENU_X, MENU_Y)
    @menu.bitmap.draw_text(0, 0, MENU_X, ENTRY_HEIGHT - 1, tr("Start"))
    @menu.bitmap.draw_text(0, ENTRY_HEIGHT, MENU_X, ENTRY_HEIGHT - 1, tr("Settings"))
    @menu.bitmap.draw_text(0, ENTRY_HEIGHT * 2, MENU_X, ENTRY_HEIGHT - 1, tr("Exit"))
    if $game_switches[160] && $game_switches[152]
        @menu.bitmap.draw_text(0, ENTRY_HEIGHT * 3, 150, 24, tr("..."))
    end
    @menu.x = Graphics.width - MENU_X
    @menu.y = Graphics.height - MENU_Y
    
    @debug = Sprite.new(@viewport)
    @debug.x = 5
    @debug.y = 5
    @debug.z += 1
    @debug.visible = Settings[:debug_text_scene_title] || false
    @debug.bitmap = Bitmap.new(200, ENTRY_HEIGHT * 6)
    @debug.bitmap.draw_text(0, 0, 200, ENTRY_HEIGHT, tr("Ruby #{RUBY_VERSION}"))
    @debug.bitmap.draw_text(0, ENTRY_HEIGHT, 200, ENTRY_HEIGHT, tr("SDL #{SDLVer}"))
    @debug.bitmap.draw_text(0, ENTRY_HEIGHT * 2, 200, ENTRY_HEIGHT, tr("Sunshine #{SunshineVer}"))
    @debug.bitmap.draw_text(0, ENTRY_HEIGHT * 3, 200, ENTRY_HEIGHT, tr("sec_#{Sunshine::SECURITYSTATE}"))
    jit_text = "JIT: unsupported"
    if defined?(RubyVM::YJIT) && RubyVM::YJIT.enabled?
      jit_text = "JIT: YJIT"
    elsif defined?(RubyVM::ZJIT) && RubyVM::ZJIT.enabled?
      jit_text = "JIT: ZJIT"
    elsif defined?(RubyVM::RJIT) && RubyVM::RJIT.enabled?
      jit_text = "JIT: RJIT"
    end
    @debug.bitmap.draw_text(0, ENTRY_HEIGHT * 4, 200, ENTRY_HEIGHT, tr(jit_text))
    if ModLoader::IS_ENABLED
      @debug.bitmap.draw_text(0, ENTRY_HEIGHT * 5, 200, ENTRY_HEIGHT, tr("Mods loaded: #{ModLoader::COUNT}"))
    end

    Language.register_text_sprite(self.class.name + "_contents", @menu.bitmap)

    # Make cursor graphic
    @cursor = Sprite.new(@viewport)
    @cursor.bitmap = RPG::Cache.menu('cursor')
    @cursor.zoom_x = @cursor.zoom_y = 2
    @cursor.z += 2
    @cursor.x = Graphics.width - MENU_X - 12
    @cursor.y = Graphics.height - MENU_Y + (ENTRY_HEIGHT - @cursor.bitmap.height) / 2 - 2
	
    # Initialize cursor position
    @cursor_pos = 0.0

    @update_connection = Graphics.viewport_resized do |w, h|
      @viewport.rect.width = w
      @viewport.rect.height = h
      
      @menu.x = w - MENU_X
      @menu.y = h - MENU_Y
      @cursor.x = w - MENU_X - 12
      @cursor.y = ((h - MENU_Y).to_f + (ENTRY_HEIGHT - @cursor.bitmap.height) / 2.0 + ENTRY_HEIGHT * @cursor_pos - 2)
      
      @sprite.x = w / 2
      @sprite.y = h / 2
      px = (Graphics.width / 1920.0)
      py = (Graphics.height / 1080.0)
      @sprite.ox = @sprite.bitmap.width / 2 * (0.5 * (1.0 - px) + px)
      @sprite.oy = @sprite.bitmap.height / 2 * (1.3 * (1.0 - py) + py)
    end

    # Play title BGM
    if File.exist?("badend.lock")
      Audio.bgm_play("Audio/BGM/MyBurdenIsDead.ogg", Audio.bgm_volume, 100)
    else
      $game_system.bgm_play($data_system.title_bgm)
    end
    # Stop playing ME and BGS
    Audio.me_stop
    Audio.bgs_stop
    
    # Execute transition
    Graphics.transition(40)
    # Main loop
    while true
      # Update game screen
      Graphics.update
      # Update input information
      Input.update
      # Frame update
      update
	    @window_settings_title.update
      # Abort loop if screen is changed
      if $scene != self
        break
      end
    end
    # Prepare for transition
    Graphics.freeze
    # Dispose of title graphic
    @update_connection.disconnect
    @sprite.bitmap.dispose
    @sprite.dispose
    @menu.bitmap.dispose
    @menu.dispose
    @cursor.bitmap.dispose
    @cursor.dispose
    @debug.bitmap.dispose
    @debug.dispose
    @viewport.dispose
    @window_settings_title.dispose
    Audio.bgm_fade(60)
    Graphics.transition(60)
    # Run automatic change for BGM and BGS set with map
    $game_map.autoplay
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    @debug.visible = Settings[:debug_text_scene_title] || false # if undefined don't render
    
    if Input.trigger?(Input::F8)
      Graphics.fullscreen = $console = !Settings[:fullscreen]
      Settings[:fullscreen] = !Settings[:fullscreen]
      if @window_settings_title.visible
        @window_settings_title.redraw_setting(1, 0)
      end
    end

    # Handle cursor movement
    if !@window_settings_title.visible
      @cursor.y = ((Graphics.height - MENU_Y).to_f + (ENTRY_HEIGHT - @cursor.bitmap.height) / 2.0 + ENTRY_HEIGHT * @cursor_pos - 2) * 0.65 + @cursor.y.to_f * 0.35
      update_cursor = false
      if Input.trigger?(Input::UP)
        if @cursor_pos > 0
          @cursor_pos -= 1
          update_cursor = true
        end
      elsif Input.trigger?(Input::DOWN)
      
        if $game_switches[160] && $game_switches[152]
          if @cursor_pos < 3
            @cursor_pos += 1
            update_cursor = true
          end
        else
          if @cursor_pos < 2
            @cursor_pos += 1
            update_cursor = true
          end
        end
      end
      if update_cursor
        Audio.se_play('Audio/SE/title_cursor.wav', 40)
      end
      
      # Handle confirmation
      if Input.trigger?(Input::ACTION)
        if File.exist?("badend.lock")
          case @cursor_pos
          when 0  # Continue
            print("You killed niko.")
          when 1  # Settings
            command_settings
          when 2  # Shutdown
            command_shutdown
          end
        else
          case @cursor_pos
          when 0  # Continue
            $game_switches[157] = false
            command_continue
          when 1  # Settings
            command_settings
          when 2  # Shutdown
            command_shutdown
          when 3  # memory
            $game_switches[157] = true
            command_continue
          end
        end
      end
    end
  end
  #--------------------------------------------------------------------------
  # * initialize a new game
  #--------------------------------------------------------------------------
  def new_game
    # Reset frame count for measuring play time
    Graphics.frame_count = 0
    # Make each type of game object
    $game_system        = Game_System.new
    $game_switches      = Game_Switches.new
    $game_switches[400] = Graphics.width > 1000
    $game_variables     = Game_Variables.new
    $game_self_switches = Game_SelfSwitches.new
    $game_screen        = Game_Screen.new
    $game_actors        = Game_Actors.new
    $game_party         = Game_Party.new
    $game_map           = Game_Map.new
    $game_player        = Game_Player.new
    $game_followers     = []
    $game_oneshot       = Game_Oneshot.new
    $game_fasttravel    = Game_FastTravel.new
    $light              = Game_Light.new
    # Set up initial party
    $game_party.setup_starting_members
    # Set up initial map position
    $game_map.setup(1)
    # Move player to initial position
    $game_player.moveto($data_system.start_x, $data_system.start_y)
    # Refresh player
    $game_player.refresh
  end
  #--------------------------------------------------------------------------
  # * Command: Continue
  #--------------------------------------------------------------------------
  def command_continue
    # Reset frame count for measuring play time
    Graphics.frame_count = 0
    # Play decision SE
    Audio.se_play('Audio/SE/title_decision.wav')
    # Update map (run parallel process event)
	  Oneshot.allow_exit false
    $game_map.update
    # Switch to map screen
    $scene = Scene_Map.new
  end
  #--------------------------------------------------------------------------
  # * Command: Shutdown
  #--------------------------------------------------------------------------
  def command_shutdown
    # Play decision SE
    Audio.se_play('Audio/SE/title_decision.wav')
    # Fade out BGM, BGS, and ME
    Audio.bgm_fade(800)
    Audio.bgs_fade(800)
    Audio.me_fade(800)
    # Shutdown
    Oneshot.exiting true
    $scene = nil
  end
  
  def command_settings
    $game_system.se_play($data_system.decision_se)
    @window_settings_title.open
  end
end
