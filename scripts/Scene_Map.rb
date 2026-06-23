#==============================================================================
# ** Scene_Map
#------------------------------------------------------------------------------
#  This class performs map screen processing.
#==============================================================================

class Scene_Map
  attr_accessor :in_game_timer
  #--------------------------------------------------------------------------
  # * Main Processing
  #--------------------------------------------------------------------------
  def main
    @in_game_timer = Sprite.new
    @in_game_timer.x = 8;
    @in_game_timer.y = 8;
    @in_game_timer.bitmap = Bitmap.new(140, 30)
    @in_game_timer.bitmap.fill_rect(0, 0, 140, 30, Color.new(0, 0, 0))
    @in_game_timer.z = 10001
    @in_game_timer.visible = $game_temp.igt_timer_visible
    
    # Make sprite set
    @spriteset = Spriteset_Map.new
    # Make message window
    @message_window = Window_Message.new
    @ed_message = Ed_Message.new
    @doc_message = Doc_Message.new
    @desktop_message = Desktop_Message.new
    @credits_message = Credits_Message.new
    # Make menus
    @menu = Window_MainMenu.new
    @item_menu = Window_Item.new
    @item_menu_refresh = false
    # Make item icon
    @item_icon = Sprite.new
    @item_icon.x = Graphics.width - 64
    @item_icon.y = Graphics.height - 64
    @item_icon.z = 9000
    @item_icon.zoom_x = 2.0
    @item_icon.zoom_y = 2.0
    @item_id = 0
    # Make item flash (for clover)
    @item_icon_flash = Sprite.new
    @item_icon_flash.x = Graphics.width - 64
    @item_icon_flash.y = Graphics.height - 64
    @item_icon_flash.z = 10000
    @item_icon_flash.zoom_x = 2.0
    @item_icon_flash.zoom_y = 2.0
    @item_icon_flash.opacity = 0
    @item_icon_flash_fadein = true
    @item_icon_flash_wait = 100
    # Make fast travel menu
    @fast_travel = FastTravel.new
    @window_settings = Window_Settings.new
    @window_debug = Window_TPtL.new
    # Fade to black transition
    @blackfade = Sprite.new
    @blackfade.bitmap = Bitmap.new(Graphics.width, Graphics.height)
    @blackfade.bitmap.fill_rect(0, 0, Graphics.width, Graphics.height, Color.new(0, 0, 0))
    @blackfade.visible = false
    @blackfade.z = 9999

	RPG::Mod.exec_hooks("hooks/Scene_Map/main", binding)
    
    # Transition run
    Graphics.transition
    # Main loop
    while true
      # Update game screen
      Graphics.update
      # Update input information
      Input.update
      # Frame update
      update
      # Abort loop if screen is changed
      if $scene != self
        break
      end
    end
    # Prepare for transition
    Graphics.freeze
    # Dispose of sprite set
    @spriteset.dispose
    # Dispose of message window
    @message_window.dispose
    @ed_message.dispose
    @doc_message.dispose
    @desktop_message.dispose
    @credits_message.dispose
    # Dispose of menu
    @menu.dispose
    @item_menu.dispose
    @fast_travel.dispose
    @window_settings.dispose
    @window_debug.dispose
    # Dispose of item icon
    @item_icon.dispose
    @item_icon_flash.dispose
    @blackfade.dispose
    @in_game_timer.dispose
    # If switching to title screen
    if $scene.is_a?(Scene_Title)
      # Fade out screen
      Graphics.transition
      Graphics.freeze
    end
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    if $game_temp.igt_timer_visible && (Graphics.frame_count != nil)
      total_sec = Graphics.frame_count.to_f / Graphics.frame_rate
      time_string = sprintf("%02d:%02d:%02d.%03d",
                            total_sec / 3600,
                            total_sec / 60 % 60,
                            total_sec % 60,
                            total_sec * 1000 % 1000)

      @in_game_timer.bitmap.fill_rect(0, 0, 140, 30, Color.new(0, 0, 0, 128))
      @in_game_timer.bitmap.draw_text(8, 0, 132, 30, time_string)
    end
  
    if Input.quit?
      # put in dialogue boxes here for player
      # either telling them they can't quit during a cutscene
      # or telling them they're saving and quitting
      if $game_system.map_interpreter.running?
        EdText.info(tr("You cannot perform this action during cutscenes."))
        return
      else
        if Settings[:oneshot_mode] == true
          File.new("badend.lock", "w")
          $game_temp.common_event_id = 35
        else
          $game_temp.common_event_id = 35
        end
      end
    end
    # Loop
    while true
      if $game_temp.prompt_wait > 0
        $game_temp.prompt_wait -= 1
        return
      end
      $game_temp.bgm_fadein($game_system)
      # Update map, interpreter, and player order
      # (this update order is important for when conditions are fulfilled
      # to run any event, and the player isn't provided the opportunity to
      # move in an instant)
      $game_map.update
      if $scene == nil
        return
      end
      $game_system.map_interpreter.update
      $game_temp.menus_visible = @menu.visible || @item_menu.visible || @fast_travel.visible || @window_settings.visible || @window_debug.visible
      $game_player.update
      $game_followers.each{|f| f.update}
      # Update system (timer), screen
      $game_system.update
      $game_screen.update
      # Abort loop if player isn't place moving
      unless $game_temp.player_transferring
        break
      end
      # Run place move
      transfer_player
      # Abort loop if transition processing
      if $game_temp.transition_processing
        break
      end
    end
    # Update sprite set
    @spriteset.update
    # Update message window
    @message_window.update
    @ed_message.update
    @doc_message.update
    @desktop_message.update
    @credits_message.update
    # Deactivate item
    if Input.trigger?(Input::DEACTIVATE) && $game_variables[1] > 0
      $game_system.se_play($data_system.cancel_se)
      $game_variables[1] = 0
    end

    # Update the menu
    if @message_window.visible || @ed_message.visible || @doc_message.visible || @desktop_message.visible || @credits_message.visible
      @item_menu_refresh = true
    else
      if @item_menu_refresh
        @item_menu_refresh = false
        @item_menu.refresh
      end
      @menu.update
      @item_menu.update
    end
    # Update the item icon
    if @item_id != $game_variables[1]
      @item_id = $game_variables[1]
      if @item_id == 0
        @item_icon.bitmap = nil
        @item_icon_flash.bitmap = nil
      else
        name = $data_items[@item_id].icon_name
        translation_name = "#{$persistent.langcode}/#{name}"
        if File.exist?("Graphics/Icons/#{translation_name}.png")
          @item_icon.bitmap = RPG::Cache.icon(translation_name)
        else
          @item_icon.bitmap = RPG::Cache.icon(name)
        end
        @item_icon.zoom_x = 2.0
        @item_icon.zoom_y = 2.0
	  
        if @item_id == 58 #clover
          @item_icon_flash.bitmap = RPG::Cache.icon($data_items[@item_id].icon_name + "2")
          @item_icon_flash.opacity = 0
          @item_icon_flash_fadein = true
          @item_icon_flash_wait = 80
        else
          @item_icon_flash.bitmap = nil
        end
      end
    end
    # Hide icon when item menu is visible
    @item_icon.visible = !@item_menu.visible
    @item_icon_flash.visible = @item_icon.visible

    if @item_icon_flash_wait > 0
      @item_icon_flash_wait -= 1
    else
      if @item_icon_flash_fadein
        @item_icon_flash.opacity += 6
        if @item_icon_flash.opacity >= 255
          @item_icon_flash.opacity = 255
          @item_icon_flash_fadein = false
          @item_icon_flash_wait = 40
        end
      else
        @item_icon_flash.opacity -= 6
        if @item_icon_flash.opacity <= 0
          @item_icon_flash.opacity = 0
          @item_icon_flash_fadein = true
          @item_icon_flash_wait = 160
        end
      end
    end
    # If returning to title screen
    if $game_temp.to_title
      # Change to title screen
      $scene = Scene_Title.new
      return
    end
    # If transition processing
    if $game_temp.transition_processing
      # Clear transition processing flag
      $game_temp.transition_processing = false
      # Execute transition
      if $game_temp.transition_name == ""
        Graphics.transition(20)
      elsif $game_temp.transition_name == "black"
        @blackfade.visible = true
        Graphics.transition(30)
        Graphics.freeze
        @blackfade.visible = false
        Graphics.transition(30)
      else
        Graphics.transition(40, "Graphics/Transitions/" +
          $game_temp.transition_name)
      end
    end
    # Update fast travel
    @fast_travel.update
    @window_settings.update
    @window_debug.update

    if Input.trigger?(Input::F8) && !$game_switches[123]
      if Graphics.fullscreen == true
      Graphics.fullscreen = false
      $console = false
      else
        Graphics.fullscreen = true
    	$console = true
      end
      sleep(0.500)
      if @window_settings.visible
        @window_settings.redraw_setting_index(2)
        sleep(0.500)
      end
    end
    # If showing message window
    if $game_temp.message_window_showing || @ed_message.visible || @doc_message.visible || @desktop_message.visible || @credits_message.visible
      return
    end
    # Process menu opening
    unless $game_system.map_interpreter.running? ||
        $game_system.menu_disabled ||
        @fast_travel.visible || @window_settings.visible || @window_debug.visible || $game_temp.menu_calling == true  || $game_temp.item_menu_calling == true
      if !@menu.visible && Input.trigger?(Input::MENU)
        $game_temp.menu_calling = true
        $game_temp.menu_beep = true
      elsif !@item_menu.visible && Input.trigger?(Input::ITEMS) && ($game_switches[174] == false)
        $game_temp.item_menu_calling = true
        $game_temp.menu_beep = true
      end
    end
    if Settings[:debug] and Input.press?(Input::F5) and 
      $game_temp.player_transferring = true
      $game_temp.player_new_map_id = 1
      $game_temp.player_new_x = $data_system.start_x
      $game_temp.player_new_y = $data_system.start_y
    end
    # If debug mode is ON and F9 key was pressed
    if Input.press?(Input::F9) && Settings[:debug] == true
       # Set debug calling flag
       $game_temp.debug_calling = true
    end
    # If player is not moving
    unless $game_player.moving?
      # Run calling of each screen
      if $game_temp.name_calling
        $game_temp.name_calling = false
        $game_player.straighten
        $scene = Scene_Name.new
      elsif $game_temp.menu_calling
        $game_temp.menu_calling = false
        if $game_temp.menu_beep
          $game_system.se_play($data_system.decision_se)
          $game_temp.menu_beep = false
        end
        $game_player.straighten
        @menu.open
      elsif $game_temp.item_menu_calling
        $game_temp.item_menu_calling = false
        if $game_temp.menu_beep
          $game_system.se_play($data_system.decision_se)
          $game_temp.menu_beep = false
        end
        $game_player.straighten
        @item_menu.open
      elsif $game_temp.travel_menu_calling
        $game_temp.travel_menu_calling = false
        $game_player.straighten
        @fast_travel.open
      elsif $game_temp.window_settings_calling
        $game_temp.window_settings_calling = false
        $game_player.straighten
        @window_settings.open
      elsif $game_temp.window_debug_calling
        $game_temp.window_debug_calling = false
        $game_player.straighten
        @window_debug.open
      elsif $game_temp.save_calling
        call_save
      elsif $game_temp.debug_calling
        $game_temp.debug_calling = false
        $game_system.se_play($data_system.decision_se)
        $game_player.straighten
        $scene = Scene_Debug.new
      end
    end
  end

  #--------------------------------------------------------------------------
  # * Player Place Move
  #--------------------------------------------------------------------------
  def transfer_player
    # Clear player place move call flag
    $game_temp.player_transferring = false
    # If move destination is different than current map
    if $game_map.map_id != $game_temp.player_new_map_id
      # Set up a new map
      $game_map.setup($game_temp.player_new_map_id)
    end
    # Set up player/follower positions
    [$game_player].concat($game_followers).each do |character|
      character.moveto($game_temp.player_new_x, $game_temp.player_new_y)
      # Set player direction
      case $game_temp.player_new_direction
      when 2  # down
        character.turn_down
      when 4  # left
        character.turn_left
      when 6  # right
        character.turn_right
      when 8  # up
        character.turn_up
      end
      # Straighten player position
      character.straighten
    end
    # Remake sprite set
    @spriteset.dispose
    @spriteset = Spriteset_Map.new
    # Update map (run parallel process event)
    $game_map.update
    @spriteset.update
    # Run automatic change for BGM and BGS set on the map
    $game_map.autoplay
    # Frame reset
    Graphics.frame_reset
  end
  #--------------------------------------------------------------------------
  # * Lighting operations
  #--------------------------------------------------------------------------
  def add_light(id, filename, intensity, x, y)
    @spriteset.add_light(id, filename, intensity, x, y)
  end
  def del_light(id)
    @spriteset.del_light(id)
  end
  def clear_lights
    @spriteset.clear_lights
  end
  #--------------------------------------------------------------------------
  # * Particle operations
  #--------------------------------------------------------------------------
  def particles=(val)
    @spriteset.particles = val
  end
  #--------------------------------------------------------------------------
  # * Follower operations
  #--------------------------------------------------------------------------
  def add_follower(follower)
    @spriteset.add_follower(follower)
  end
  def remove_follower(follower)
    @spriteset.remove_follower(follower)
  end
  #--------------------------------------------------------------------------
  # * BG operations
  #--------------------------------------------------------------------------
  def bg=(name)
    @spriteset.bg = name
  end
  #--------------------------------------------------------------------------
  # * Misc operations
  #--------------------------------------------------------------------------
  def new_footprint(direction, x, y)
    if @spriteset != nil
      @spriteset.new_footprint(direction, x, y)
	end
  end
  def new_footsplash(direction, x, y)
    if @spriteset != nil
      @spriteset.new_footsplash(direction, x, y)
	end
  end
  def new_maptext(text, x, y)
    if @spriteset != nil
      @spriteset.new_maptext(text, x, y)
	end
  end
  def fix_footsplashes(xDelt, yDelt)
    if @spriteset != nil
      @spriteset.fix_footsplashes(xDelt, yDelt)
	end
  end
  def menu_open?
    @menu.visible || @item_menu.visible
  end
end
