#==============================================================================
# ** Scene_Title
#------------------------------------------------------------------------------
#  This class performs title screen processing.
#==============================================================================

class Scene_Title
  MENU_X = 150
  MENU_Y = 100
  ENTRY_HEIGHT = 25
  SDLVer = ""

  class BG_Particle
    DISTANCE = 600.0

    def dist(x1, y1, x2, y2)
      x = x1 - x2
      y = y1 - y2
      Math.sqrt(x*x + y*y)
    end

    def initialize(viewport, bitmap)
      @sprite = Sprite.new(viewport)
      @sprite.bitmap = bitmap
      @sprite.smooth = true
      @sprite.origin_x = 1
      @sprite.origin_y = 1
      @sprite.scale_x = 32
      @sprite.scale_y = 32
      @sprite.x = -Graphics.width/2 + rand(0..Graphics.width)
      @sprite.y = -Graphics.height/2 + rand(0..Graphics.height)
      @sprite.z = viewport.origin_z
      @sprite.perspective = true
      @sprite.rot_z = rand(-180..180)
      @sprite.opacity = 0
      @sprite.order = -1

      @rot_speed = rand(-20..20) / 80.0
      @speed_x = rand(-10..10) / 70.0
      @speed_y = rand(-10..10) / 70.0
      @pos_x = @sprite.x
      @pos_y = @sprite.y

      @timer = 0
    end

    def dispose
      @sprite.dispose
    end

    def disposed?
      @sprite.disposed?
    end

    def update
      return if disposed?

      @pos_x += @speed_x
      @pos_y += @speed_y
      @sprite.x = @pos_x
      @sprite.y = @pos_y
      @sprite.rot_z += @rot_speed
      
      dist = dist(@pos_x, @pos_y, -Graphics.width/6, 0)
      #173,37,74
      #34,5,41
      if dist < DISTANCE
        @sprite.color.set(lerp(173, 34, dist / DISTANCE), lerp(37, 5, dist / DISTANCE), lerp(74, 41, dist / DISTANCE))
      else
        dist -= DISTANCE
        @sprite.color.set(lerp(34, 0, dist / DISTANCE), lerp(5, 0, dist / DISTANCE), lerp(41, 0, dist / DISTANCE))
      end

      @timer += 1

      @sprite.opacity += @timer > 512 ? -2 : 2
      @sprite.dispose if @sprite.opacity <= 0
    end
  end
  
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
 
    new_game
    
    Language.initialize_database
    Settings.load!
    Oneshot.allow_exit true
    load_perma_flags

    # Make title graphic
    
    @window_settings_title = Window_Settings.new

    @bg_viewport = Viewport.new(0, 0, Graphics.width, Graphics.height)
    @bg_viewport.origin_x = -Graphics.width / 2
    @bg_viewport.origin_y = -Graphics.height / 2
    @bg_viewport.origin_z = 200
    
    @viewport = Viewport.new(0, 0, Graphics.width, Graphics.height)

    Graphics.fov = 40

    @animated = TowerAnimation.new(@bg_viewport, @viewport)
    
    @badend_sprite = Sprite.new(@bg_viewport)
    @badend_sprite.scale_x = @badend_sprite.scale_y = 2.0
    @badend_sprite.visible = false

    # main bg
    
    @background_sprite = Sprite.new(@bg_viewport)
    @background_sprite.bitmap = RPG::Cache.title("normal_bg")
    @background_sprite.scale_x = 2.2
    @background_sprite.scale_y = 2.0
    @background_sprite.perspective = true
    @background_sprite.z = @bg_viewport.origin_z
    @background_sprite.order = -2

    @background_sprite_shadow = Sprite.new(@bg_viewport)
    @background_sprite_shadow.bitmap = RPG::Cache.title("normal_back_shadow")
    @background_sprite_shadow.scale_x = @background_sprite_shadow.scale_y = 2.0
    @background_sprite_shadow.perspective = true
    @background_sprite_shadow.z = @bg_viewport.origin_z

    @background_sprite_water_back = Sprite.new(@bg_viewport)
    @background_sprite_water_back.bitmap = Bitmap.new(412, 388)
    @background_sprite_water_back.bitmap.stretch_blt(Rect.new(0, 0, 412, 388), RPG::Cache.title("tower"), Rect.new(0, 240, 412, 388))
    @background_sprite_water_back.scale_x = @background_sprite_water_back.scale_y = 2.0
    @background_sprite_water_back.perspective = true
    @background_sprite_water_back.z = @bg_viewport.origin_z
    @background_sprite_water_back.origin_x = 206
    @background_sprite_water_back.origin_y = 194
    @background_sprite_water_back.rot_x = 90
    #@background_sprite_water_back.smooth = true

    @background_sprite_water_light = Sprite.new(@bg_viewport)
    @background_sprite_water_light.bitmap = RPG::Cache.title("normal_water_light")
    @background_sprite_water_light.scale_x = @background_sprite_water_light.scale_y = 2.0
    @background_sprite_water_light.perspective = true
    @background_sprite_water_light.z = @bg_viewport.origin_z
    @background_sprite_water_light.blend_type = 1

    @background_sprite_niko = Sprite.new(@bg_viewport)
    @background_sprite_niko.bitmap = RPG::Cache.title("normal_niko")
    @background_sprite_niko.scale_x = @background_sprite_niko.scale_y = 2.0
    @background_sprite_niko.perspective = true
    @background_sprite_niko.z = @bg_viewport.origin_z

    @background_sprite_light = Sprite.new(@bg_viewport)
    @background_sprite_light.bitmap = RPG::Cache.title("normal_light")
    @background_sprite_light.scale_x = @background_sprite_light.scale_y = 2.0
    @background_sprite_light.perspective = true
    @background_sprite_light.z = @bg_viewport.origin_z
    @background_sprite_light.blend_type = 1

    # main bg paricles
    
    @particles = []
    @particle_bitmap = Bitmap.new(2, 2)
    @particle_bitmap.fill_rect(0, 0, 2, 1, Color.new(255, 255, 255))
    @particle_bitmap.fill_rect(0, 1, 2, 1, Color.new(255, 255, 255, 0))

    # Create/render menu options
    @menu = Sprite.new(@viewport)
    @menu.z += 1
    @menu.bitmap = Bitmap.new(MENU_X, MENU_Y)
    @menu.x = Graphics.width - MENU_X
    @menu.y = Graphics.height - MENU_Y
    redraw_menu
    
    @debug = Sprite.new(@viewport)
    @debug.x = 5
    @debug.y = 5
    @debug.z += 1
    @debug.visible = Settings[:debug_text_scene_title] || false
    @debug.bitmap = Bitmap.new(400, ENTRY_HEIGHT * 6)
    redraw_debug

    Language.register_text_sprite(self.class.name + "_contents", @menu)
    Language.register_text_sprite(self.class.name + "_debug", @debug)

    # Make cursor graphic
    @cursor = Sprite.new(@viewport)
    @cursor.bitmap = RPG::Cache.menu('cursor')
    @cursor.scale_x = @cursor.scale_y = 2
    @cursor.z += 2
    @cursor.x = Graphics.width - MENU_X - 12
    @cursor.y = Graphics.height - MENU_Y + (ENTRY_HEIGHT - @cursor.bitmap.height) / 2 - 3
	
    # chinese has its own special title screen so check for it
    #translation_name = "#{$persistent.langcode}/#{$data_system.title_name}"
    #if File.exist?("Graphics/Titles/#{translation_name}.png")
    #  @background_sprite.bitmap = RPG::Cache.title(translation_name)
    #else
    #  @background_sprite.bitmap = RPG::Cache.title($data_system.title_name)
    #end

    # check for debug file to add debug items
    if Settings[:debug]
      $game_party.gain_item(54, 1) # debug save
        $game_party.gain_item(82, 1) # plight skip
      $game_party.gain_item(81, 1) # George reroler
    end
    
    # init update
    redraw_bgs

    # Initialize cursor position
    @cursor_pos = 0.0

    @update_connection = Graphics.viewport_resized do |w, h|
      @viewport.rect.width = w
      @viewport.rect.height = h
      @bg_viewport.rect.width = w
      @bg_viewport.rect.height = h
      
      @bg_viewport.origin_x = -Graphics.width / 2
      @bg_viewport.origin_y = -Graphics.height / 2
      @bg_viewport.origin_z = 200

      @menu.x = w - MENU_X
      @menu.y = h - MENU_Y
      @cursor.x = w - MENU_X - 12
      @cursor.y = ((h - MENU_Y).to_f + (ENTRY_HEIGHT - @cursor.bitmap.height) / 2.0 + ENTRY_HEIGHT * @cursor_pos - 3)

      redraw_bgs
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
      # only for legacy funcs
      Audio.update
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

    @animated.dispose if !@animated.disposed?

    @background_sprite.bitmap.dispose
    @background_sprite.dispose
    @background_sprite_shadow.bitmap.dispose
    @background_sprite_shadow.dispose
    @background_sprite_water_back.bitmap.dispose
    @background_sprite_water_back.dispose
    @background_sprite_water_light.bitmap.dispose
    @background_sprite_water_light.dispose
    @background_sprite_niko.bitmap.dispose
    @background_sprite_niko.dispose
    @background_sprite_light.bitmap.dispose
    @background_sprite_light.dispose

    @badend_sprite.bitmap.dispose
    @badend_sprite.dispose

    @menu.bitmap.dispose
    @menu.dispose

    @cursor.bitmap.dispose
    @cursor.dispose

    @debug.bitmap.dispose
    @debug.dispose
    
    @particle_bitmap.dispose
    @particles.each { |particle| particle.dispose }

    @viewport.dispose
    @bg_viewport.dispose

    @window_settings_title.dispose

    Audio.bgm_fade(60)
    Graphics.transition(60)
    # Run automatic change for BGM and BGS set with map
    $game_map.autoplay
  end

  def redraw_menu
    if !@menu
      return
    end

    @menu.bitmap.clear
    @menu.bitmap.draw_text(0, 0, MENU_X, ENTRY_HEIGHT - 1, tr("Start"))
    @menu.bitmap.draw_text(0, ENTRY_HEIGHT, MENU_X, ENTRY_HEIGHT - 1, tr("Settings"))
    @menu.bitmap.draw_text(0, ENTRY_HEIGHT * 2, MENU_X, ENTRY_HEIGHT - 1, tr("Exit"))
    if $game_switches[160] && $game_switches[152]
        @menu.bitmap.draw_text(0, ENTRY_HEIGHT * 3, 150, 24, tr("..."))
    end
  end

  def redraw_debug
    if !@debug
      return
    end

    @debug.bitmap.clear
    @debug.bitmap.draw_text(0, 0, 400, ENTRY_HEIGHT, "Ruby #{RUBY_VERSION}")
    @debug.bitmap.draw_text(0, ENTRY_HEIGHT, 400, ENTRY_HEIGHT, "SDL #{Sunshine::SDLVersion_major}.#{Sunshine::SDLVersion_minor}.#{Sunshine::SDLVersion_micro}")
    @debug.bitmap.draw_text(0, ENTRY_HEIGHT * 2, 400, ENTRY_HEIGHT, "Sunshine #{Sunshine::VERSION}")
    @debug.bitmap.draw_text(0, ENTRY_HEIGHT * 3, 400, ENTRY_HEIGHT, "Render backend: #{Sunshine::RENDERER}")
    @debug.bitmap.draw_text(0, ENTRY_HEIGHT * 4, 400, ENTRY_HEIGHT, "Render API: #{Sunshine::RENDER_API}")
    if ModLoader::IS_ENABLED
      @debug.bitmap.draw_text(0, ENTRY_HEIGHT * 5, 400, ENTRY_HEIGHT, tr("Mods loaded: ") + ModLoader::COUNT.to_s)
      @debug.bitmap.draw_text(0, ENTRY_HEIGHT * 6, 400, ENTRY_HEIGHT, tr("PreloadScripts loaded: ") + ModLoader::PRELOAD_SCRIPTS_COUNT.to_s)
    end
  end

  def redraw_bgs
    px = (Graphics.width / 1920.0)
    py = (Graphics.height / 1080.0)
    
    @background_sprite.origin_x =
    @background_sprite_shadow.origin_x =
    @background_sprite_water_light.origin_x =
    @background_sprite_niko.origin_x =
    @background_sprite_light.origin_x =
    @background_sprite_niko.bitmap.width / 2 * (0.5 * (1.0 - px) + px)

    @background_sprite.origin_y =
    @background_sprite_shadow.origin_y =
    @background_sprite_water_light.origin_y =
    @background_sprite_niko.origin_y =
    @background_sprite_light.origin_y =
    @background_sprite_niko.bitmap.height / 2 * (1.3 * (1.0 - py) + py)

    @background_sprite_water_back.x = @background_sprite_niko.origin_x - 400
    @background_sprite_water_back.y = @background_sprite_niko.origin_y + 50

    if @badend_sprite.bitmap
      @badend_sprite.bitmap.dispose
    end
    @badend_sprite.bitmap = RPG::Cache.title("badend")

    @badend_sprite.origin_x = @badend_sprite.bitmap.width / 2
    @badend_sprite.origin_y = @badend_sprite.bitmap.height / 2
    @badend_sprite.x = Graphics.width / 2
    @badend_sprite.y = Graphics.height / 2
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    @debug.visible = Settings[:debug_text_scene_title] || false # if undefined don't render

    @background_sprite_water_back.rot_y += 0.04

    @particles.delete_if { |particle| particle.disposed? }
    @particles << BG_Particle.new(@bg_viewport, @particle_bitmap)
    @particles << BG_Particle.new(@bg_viewport, @particle_bitmap)
    @particles << BG_Particle.new(@bg_viewport, @particle_bitmap)
    @particles.each { |particle| particle.update }
    #puts @particles.length

    if !@animated.animation_ended
      @animated.update
      return
    elsif !@animated.disposed?
      @animated.dispose
    end
    
    if Input.trigger?(Input::F8)
      Graphics.fullscreen = $console = !Settings[:fullscreen]
      Settings[:fullscreen] = !Settings[:fullscreen]
      if @window_settings_title.visible
        @window_settings_title.redraw_setting(1, 0)
      end
    end

    # Handle cursor movement
    if !@window_settings_title.visible
      @cursor.y = ((Graphics.height - MENU_Y).to_f + (ENTRY_HEIGHT - @cursor.bitmap.height) / 2.0 + ENTRY_HEIGHT * @cursor_pos - 3) * 0.65 + @cursor.y.to_f * 0.35
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
    $game_temp          = Game_Temp.new
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

  def redraw
    redraw_menu
    redraw_debug
    @window_settings_title&.redraw_all
  end

  private

  class TowerAnimation
    MAX_POS = 2005.6109999999946
    TOWER_POS = -MAX_POS - 160
    TOWER_Z_OFFSET = -220

    attr_reader :animation_ended

    def initialize(viewport, overlay_viewport)
      atlas = RPG::Cache.title("tower")

      @animation_ended = false

      @timer = 0
      @speed = 0.01
      @speed_down = false

      @loaded_sound = Audio.create_sound("Audio/loaded", false, Audio.sounds_group)

      @viewport = viewport
      @viewport.origin_x = -Graphics.width / 2
      @viewport.origin_y = -Graphics.height / 2 - MAX_POS
      @viewport.origin_z = 200
      @viewport.rot_x = -30

      @overlay_viewport = overlay_viewport
      @overlay_viewport.origin_y = -MAX_POS

      @black = Sprite.new(@viewport)
      @black.bitmap = RPG::Cache.picture("black")
      @black.y = -MAX_POS
      @black.origin_x = @black.bitmap.width / 2
      @black.origin_y = @black.bitmap.height / 2
      @black.order = 2

      @floor = Sprite.new(@viewport)
      @floor.bitmap = Bitmap.new(192, 192)
      @floor.bitmap.stretch_blt(Rect.new(0, 0, 192, 192), atlas, Rect.new(0, 0, 192, 192))
      @floor.origin_x = @floor.origin_y = 96
      @floor.y = TOWER_POS
      @floor.z = TOWER_Z_OFFSET
      @floor.rot_x = 90
      
      @wall_left = Sprite.new(@viewport)
      @wall_left.bitmap = Bitmap.new(96, 240)
      @wall_left.bitmap.stretch_blt(Rect.new(0, 0, 96, 240), atlas, Rect.new(192, 0, 96, 240))
      @wall_left.origin_y = 240
      @wall_left.x = -96
      @wall_left.y = TOWER_POS
      @wall_left.z = TOWER_Z_OFFSET
      @wall_left.rot_y = 90.0 - 26.57
      @wall_left.scale_x = 1.118033989

      @wall_center = Sprite.new(@viewport)
      @wall_center.bitmap = Bitmap.new(96, 240)
      @wall_center.bitmap.stretch_blt(Rect.new(0, 0, 96, 240), atlas, Rect.new(288, 0, 96, 240))
      @wall_center.origin_x = 48
      @wall_center.origin_y = 240
      @wall_center.z = 96 + TOWER_Z_OFFSET
      @wall_center.y = TOWER_POS

      @wall_right = Sprite.new(@viewport)
      @wall_right.bitmap = Bitmap.new(96, 240)
      @wall_right.bitmap.stretch_blt(Rect.new(0, 0, 96, 240), atlas, Rect.new(384, 0, 96, 240))
      @wall_right.origin_x = 96
      @wall_right.origin_y = 240
      @wall_right.x = 96
      @wall_right.y = TOWER_POS
      @wall_right.z = TOWER_Z_OFFSET
      @wall_right.rot_y = -90.0 + 26.57
      @wall_right.scale_x = 1.118033989

      @mirror_wall_left = Sprite.new(@viewport)
      @mirror_wall_left.bitmap = @wall_left.bitmap
      @mirror_wall_left.origin_y = 240
      @mirror_wall_left.x = -96
      @mirror_wall_left.y = TOWER_POS
      @mirror_wall_left.z = TOWER_Z_OFFSET
      @mirror_wall_left.rot_y = 90.0 - 26.57
      @mirror_wall_left.scale_x = 1.118033989

      @mirror_wall_center = Sprite.new(@viewport)
      @mirror_wall_center.bitmap = @wall_center.bitmap
      @mirror_wall_center.origin_x = 48
      @mirror_wall_center.origin_y = 240
      @mirror_wall_center.z = 96 + TOWER_Z_OFFSET
      @mirror_wall_center.y = TOWER_POS

      @mirror_wall_right = Sprite.new(@viewport)
      @mirror_wall_right.bitmap = @wall_right.bitmap
      @mirror_wall_right.origin_x = 96
      @mirror_wall_right.origin_y = 240
      @mirror_wall_right.x = 96
      @mirror_wall_right.y = TOWER_POS
      @mirror_wall_right.z = TOWER_Z_OFFSET
      @mirror_wall_right.rot_y = -90.0 + 26.57
      @mirror_wall_right.scale_x = 1.118033989

      @mirror_wall_left.order = @mirror_wall_center.order = @mirror_wall_right.order =
      @mirror_wall_left.scale_y = @mirror_wall_center.scale_y = @mirror_wall_right.scale_y = -1

      @pedestal = Sprite.new(@viewport)
      @pedestal.bitmap = Bitmap.new(192, 192)
      @pedestal.bitmap.stretch_blt(Rect.new(0, 0, 48, 32), atlas, Rect.new(144, 208, 48, 32))
      @pedestal.origin_x = 24
      @pedestal.origin_y = 32
      @pedestal.y = TOWER_POS
      @pedestal.z = TOWER_Z_OFFSET

      @lamp = Sprite.new(@viewport)
      @lamp.bitmap = Bitmap.new(32, 32)
      @lamp.bitmap.stretch_blt(Rect.new(0, 0, 32, 32), atlas, Rect.new(0, 0, 32, 32))
      @lamp.scale_x = @lamp.scale_y = 0.5
      @lamp.origin_x = 16
      @lamp.origin_y = 16
      @lamp.blend_type = 1
      @lamp.rot_x = 30
      @lamp.y = TOWER_POS - 28
      @lamp.z = TOWER_Z_OFFSET
      #@lamp.smooth = true
      @lamp.order = 3

      @lamp_light = Sprite.new(@viewport)
      @lamp_light.bitmap = Bitmap.new(64, 64)
      @lamp_light.bitmap.stretch_blt(Rect.new(0, 0, 64, 64), atlas, Rect.new(416, 240, 64, 64))
      @lamp_light.scale_x = @lamp_light.scale_y = 0.5
      @lamp_light.origin_x = 32
      @lamp_light.origin_y = 32
      @lamp_light.blend_type = 1
      @lamp_light.rot_x = 30
      #@lamp_light.smooth = true
      @lamp_light.y = TOWER_POS - 28
      @lamp_light.z = TOWER_Z_OFFSET

      @base_bitmap = Bitmap.new(2, 2)
      @base_bitmap.fill_rect(0, 0, 2, 1, Color.new(10, 10, 16))
      @base_bitmap.fill_rect(0, 1, 2, 1, Color.new(0, 0, 0))
      
      @front_wall_left = Sprite.new(@viewport)
      @front_wall_left.bitmap = @base_bitmap
      @front_wall_left.x = -96
      @front_wall_left.y = TOWER_POS
      @front_wall_left.z = TOWER_Z_OFFSET
      @front_wall_left.rot_y = -90.0 + 26.57
      @front_wall_left.scale_x = 1.118033989 * 48

      @front_wall_center = Sprite.new(@viewport)
      @front_wall_center.bitmap = @base_bitmap
      @front_wall_center.origin_x = 1
      @front_wall_center.z = -96 + TOWER_Z_OFFSET
      @front_wall_center.y = TOWER_POS
      @front_wall_center.scale_x = 48

      @front_wall_right = Sprite.new(@viewport)
      @front_wall_right.bitmap = @base_bitmap
      @front_wall_right.x = 96
      @front_wall_right.y = TOWER_POS
      @front_wall_right.z = TOWER_Z_OFFSET
      @front_wall_right.rot_y = -90.0 - 26.57
      @front_wall_right.scale_x = 1.118033989 * 48

      @front_wall_left.scale_y = @front_wall_center.scale_y = @front_wall_right.scale_y = 240
      @front_wall_left.smooth = @front_wall_center.smooth = @front_wall_right.smooth = true

      @mirror_wall_left.perspective = @mirror_wall_center.perspective = @mirror_wall_right.perspective =
      @front_wall_left.perspective = @front_wall_center.perspective = @front_wall_right.perspective =
      @wall_left.perspective = @wall_center.perspective = @wall_right.perspective =
      @lamp.perspective = @lamp_light.perspective =
      @floor.perspective = @pedestal.perspective = true
      #@wall_left.smooth = @wall_center.smooth = @wall_right.smooth =
      #@floor.smooth = true
      @sum = 0.0
      #@loaded_sound.play
    end

    def update
      if disposed?
        return
      end
      if Input.pressed_key != nil || !Settings[:start_animation]
        if @timer < 16
          @loaded_sound.play
        end

        @speed = 0
        @animation_ended = true
        @timer = 32
        $game_system.bgm_play($data_system.title_bgm)

        @viewport.origin_y = -Graphics.height / 2
        @viewport.rot_x = 0
        @overlay_viewport.origin_y = 0
        return
      end
      
      if @timer == 16
        @black.visible = false
        @lamp.visible = false
        @loaded_sound.play
      end

      if @timer < 32
        @timer += 1
        return
      end

      @viewport.origin_y += @speed * 0.1
      @overlay_viewport.origin_y += @speed * 0.1

      if @speed > 0
        @speed += @speed_down ? -2.5 : 1.67
      else
        @speed = 0
        @animation_ended = true
      end

      @sum += @speed * 0.1
      #puts @sum

      if @speed > 200
        @speed_down = true
        $game_system.bgm_play($data_system.title_bgm)
      end

      @viewport.rot_x = lerp(-30.0, 0.0, @sum / MAX_POS)
    end

    def disposed?
      @floor.disposed?
    end

    def dispose
      @black.dispose
      
      @floor.bitmap.dispose
      @floor.dispose
      
      @wall_right.bitmap.dispose
      @wall_center.bitmap.dispose
      @wall_left.bitmap.dispose

      @wall_right.dispose
      @wall_center.dispose
      @wall_left.dispose
      
      @mirror_wall_right.dispose
      @mirror_wall_center.dispose
      @mirror_wall_left.dispose

      @pedestal.bitmap.dispose
      @pedestal.dispose

      @lamp.bitmap.dispose
      @lamp.dispose
      @lamp_light.bitmap.dispose
      @lamp_light.dispose

      @base_bitmap.dispose

      @front_wall_right.dispose
      @front_wall_center.dispose
      @front_wall_left.dispose
    end
  end
end