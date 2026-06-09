#==============================================================================
# ** Scene_Title
#------------------------------------------------------------------------------
#  This class performs title screen processing.
#==============================================================================

class Scene_Title
  MENU_X = Graphics.width - 150
  MENU_Y = Graphics.height - 100
  SDLVer = "#{Sunshine::SDLVersion_major}.#{Sunshine::SDLVersion_minor}.#{Sunshine::SDLVersion_micro}"
  SunshineVer = "0.1-dev"
  
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

    # Make title graphic
    @sprite = Sprite.new
	
    # chinese has its own special title screen so check for it
    translation_name = "#{$persistent.langcode}/#{$data_system.title_name}"
    if File.exist?("Graphics/Titles/#{translation_name}.png")
      @sprite.bitmap = RPG::Cache.title(translation_name)
    else
      if File.exist?("badend.lock")
        if Graphics.width == 1280
          @sprite.bitmap = RPG::Cache.title("badend_16")
        else
          @sprite.bitmap = RPG::Cache.title("badend")
        end
      else
        if Graphics.width == 1280
          @sprite.bitmap = RPG::Cache.title("normal_16")
        else
          @sprite.bitmap = RPG::Cache.title($data_system.title_name)
        end
      end
    end

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
    @menu = Sprite.new
    @menu.z += 1
    @menu.bitmap = Bitmap.new(Graphics.width, Graphics.height)
    @menu.bitmap.draw_text(MENU_X, MENU_Y, 150, 24, tr("Start"))
    @menu.bitmap.draw_text(MENU_X, MENU_Y + 25, 150, 24, tr("Settings"))
    @menu.bitmap.draw_text(MENU_X, MENU_Y + 50, 150, 24, tr("Exit"))

	#Debug info like in minecraft Forge :P
    @debug = Sprite.new
    @debug.z += @menu.z
    @debug.bitmap = Bitmap.new(Graphics.width, Graphics.height)
    @debug.bitmap.draw_text(5, 5, 200, 20, tr("Ruby #{RUBY_VERSION}"))
    @debug.bitmap.draw_text(5, 25, 200, 20, tr("SDL #{SDLVer}"))
    @debug.bitmap.draw_text(5, 45, 200, 20, tr("Sunshine #{SunshineVer}"))
    @debug.bitmap.draw_text(5, 65, 200, 20, tr("sec_#{Sunshine::SECURITYSTATE}"))
    
    if ModLoader::IS_ENABLED
    	@debug.bitmap.draw_text(5, 65, 200, 20, tr("Mods loaded: #{ModLoader::COUNT}"))
    end

    if $game_switches[160] && $game_switches[152]
        @menu.bitmap.draw_text(MENU_X, MENU_Y + 75, 150, 24, tr("..."))
    end

	Language.register_text_sprite(self.class.name + "_contents", @menu.bitmap)

    # Make cursor graphic
    @cursor = Sprite.new
    @cursor.zoom_x = @cursor.zoom_y = 2
    @cursor.bitmap
    @cursor.z += 2
    @cursor.bitmap = RPG::Cache.menu('cursor')
    @cursor.x = MENU_X - 12
    @cursor.y = MENU_Y + (20 - @cursor.bitmap.height) / 2
	
    # Initialize cursor position
    @cursor_pos = 0.0
    

    # Play title BGM
    if File.exist?("badend.lock")
      #TODO: fix audio values
      Audio.bgm_play("Audio/BGM/MyBurdenIsDead.ogg", bgm_volume, 100)
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
    @sprite.bitmap.dispose
    @sprite.dispose
    @menu.bitmap.dispose
    @menu.dispose
    @cursor.bitmap.dispose
    @cursor.dispose
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
    # Handle cursor movement
    if !@window_settings_title.visible
      @cursor.y = (MENU_Y.to_f + (20.0 - @cursor.bitmap.height.to_f) / 2.0 + 25.5 * @cursor_pos) * 0.65 + @cursor.y.to_f * 0.35
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
      if Input.trigger?(Input::F8)
        if Graphics.fullscreen == true
          Graphics.fullscreen = false
          $console = false
        else
          Graphics.fullscreen = true
          $console = true
        end
      end
      if update_cursor
        Audio.se_play('Audio/SE/title_cursor.wav', 40)
      end
    end

    if !@window_settings_title.visible
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
    # Set up initial party
    $game_party.setup_starting_members
    # Set up initial map position
    $game_map.setup($data_system.start_map_id)
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
