class FastTravel
  MARGIN = 30
  TITLE_TOP_MARGIN = -16
  TITLE_MARGIN = 100
  ITEM_SPACING = 28
  ACTIVE_MARGIN = MARGIN * 2 + 20


  def initialize
    @scale_multiplier = Graphics.height > 600 ? 3.0 : 2.0

    @viewport = Viewport.new(0, 0, Graphics.width, Graphics.height)
    @bg = Sprite.new(@viewport)
    @bg.bitmap = Bitmap.new(Graphics.width, Graphics.height)
    @bg.bitmap.fill_rect(0, 0, Graphics.width, Graphics.height, Color.new(0, 0, 0, 128))
    @title = Sprite.new(@viewport)
    Language.register_text_sprite(self.class.name + "_title", @title)
    @title.bitmap = Bitmap.new(Graphics.width / 2, TITLE_MARGIN)
    @title.bitmap.font.size = 20
    @title.y = TITLE_TOP_MARGIN
    @title.x = MARGIN
    @title.zoom_x = @title.zoom_y = 2

    # WME Location name
    @location_name_text = Sprite.new(@viewport)
    @location_name_text.bitmap = Bitmap.new(Graphics.width, 30)
    @location_name_text.y = Graphics.height - 64
    @location_name_text.bitmap.font.size = 20
    @location_name_text.opacity = 0

    # WME Niko Icon
    @niko_icon = Sprite.new(@viewport)
    @niko_icon.bitmap = RPG::Cache.load_bitmap("Graphics/Menus/Minimap/", "niko_icon")
    @niko_icon.zoom_x = @niko_icon.zoom_y = @scale_multiplier
    @niko_icon.z = 10
    @niko_icon.opacity = 0
    @niko_icon.shader = Shader::WorldMachine

    @niko_pos_x = 0.0
    @niko_pos_y = 0.0
    @niko_target_pos_x = 0.0
    @niko_target_pos_y = 0.0

    # WME Niko arrows
    arrows_bitmap = RPG::Cache.load_bitmap("Graphics/Menus/Minimap/", "small_arrows")
    @arrow_top = Sprite.new(@viewport)
    @arrow_top.bitmap = Bitmap.new(16, 16)
    @arrow_top.bitmap.stretch_blt(Rect.new(0, 0, 16, 16), arrows_bitmap, Rect.new(0, 0, 16, 16))
    
    @arrow_bottom = Sprite.new(@viewport)
    @arrow_bottom.bitmap = Bitmap.new(16, 16)
    @arrow_bottom.bitmap.stretch_blt(Rect.new(0, 0, 16, 16), arrows_bitmap, Rect.new(16, 0, 16, 16))
    
    @arrow_left = Sprite.new(@viewport)
    @arrow_left.bitmap = Bitmap.new(16, 16)
    @arrow_left.bitmap.stretch_blt(Rect.new(0, 0, 16, 16), arrows_bitmap, Rect.new(32, 0, 16, 16))
    
    @arrow_right = Sprite.new(@viewport)
    @arrow_right.bitmap = Bitmap.new(16, 16)
    @arrow_right.bitmap.stretch_blt(Rect.new(0, 0, 16, 16), arrows_bitmap, Rect.new(48, 0, 16, 16))

    @arrow_top.zoom_x = @arrow_top.zoom_y =
    @arrow_bottom.zoom_x = @arrow_bottom.zoom_y =
    @arrow_left.zoom_x = @arrow_left.zoom_y =
    @arrow_right.zoom_x = @arrow_right.zoom_y = @scale_multiplier
    
    @arrow_top.opacity = @arrow_bottom.opacity =
    @arrow_left.opacity = @arrow_right.opacity = 0
    
    @arrow_top.shader = @arrow_bottom.shader =
    @arrow_left.shader = @arrow_right.shader = Shader::WorldMachine

    @arrow_top.z = @arrow_bottom.z =
    @arrow_left.z = @arrow_right.z = 11

    @arrows_timer = 0
    @arrows_offset = 0

    # WME data
    @data_locations = {}
    @selected_location = nil

    # legacy
    @data_sprites = []

    #other
    @viewport.z = 9998

    self.visible = false
    @index = 0
    @fade_in = false
    @fade_out = false

    @transfer_player = nil
  end
  
  #WME
  def update_location_name_text
    if ZONES[$game_fasttravel.zone].locations == nil || ZONES[$game_fasttravel.zone].maps[@selected_location] == nil || (Settings[:fasttravel_ui] == 0)
      return
    end
    @location_name_text.bitmap.clear
    @location_name_text.bitmap.draw_text(0, 0, Graphics.width, 30, ZONES[$game_fasttravel.zone].maps[@selected_location], 1)
  end

  def set_color(location_name)
    if @selected_location == location_name
      @data_locations[location_name].modulate.set(255, 255, 255)
      return
    end
    case $game_fasttravel.zone
    when :blue
      @data_locations[location_name].modulate.set(84, 76, 209)
    when :green
      @data_locations[location_name].modulate.set(63, 213, 100)
    when :red, :red_ground
      @data_locations[location_name].modulate.set(213, 33, 106)
    end
  end
  
  def update_arrows_positions
    @arrow_left.y = @arrow_right.y = @niko_icon.y
    @arrow_top.x = @arrow_bottom.x = @niko_icon.x + 4 * @scale_multiplier
    
    @arrow_top.y = @niko_icon.y - (12 + @arrows_offset) * @scale_multiplier
    @arrow_bottom.y = @niko_icon.y + @niko_icon.bitmap.height * @scale_multiplier / 3 + (6 + @arrows_offset) * @scale_multiplier
    @arrow_left.x = @niko_icon.x - (8 + @arrows_offset) * @scale_multiplier
    @arrow_right.x = @niko_icon.x + @niko_icon.bitmap.width * @scale_multiplier / 3 + (7 + @arrows_offset) * @scale_multiplier
  end

  #Not WME
  def dispose
    @bg.dispose
    @title.dispose
    @data_sprites.each do |spr|
      spr.dispose
    end

    #WME
    @data_locations.each do |name, spr|
      spr.dispose
    end

    @location_name_text.dispose
    @niko_icon.dispose

    @arrow_top.dispose
    @arrow_bottom.dispose
    @arrow_left.dispose
    @arrow_right.dispose
    
    #Not WME
    @viewport.dispose
  end

  def open
    self.visible = true
    self.opacity = 0

    #WME
    if !(Settings[:fasttravel_ui] == 0)
      @niko_icon.opacity = @location_name_text.opacity =
      @arrow_top.opacity = @arrow_bottom.opacity =
      @arrow_left.opacity = @arrow_right.opacity = 0
    end

    #Not WME
    @data = $game_fasttravel.unlocked_maps.keys.sort
    zone = ZONES[$game_fasttravel.zone]

    # Set cursor to current map
    @selected_location = @data[0]
    @data.each_with_index do |map, i|
      if $game_fasttravel.unlocked_maps[map].id == $game_map.map_id
        @index = i
        @selected_location = map
        break
      end
    end
    @fade_in = true

    # Create title
    @title.bitmap.clear
    @title.bitmap.draw_text(0, 0, @title.bitmap.width, @title.bitmap.height, zone.name)

    # Create menu options
    @data.each_with_index do |item, i|
      if zone.locations != nil && !(Settings[:fasttravel_ui] == 0)
        #WME
        spr = Sprite.new(@viewport)
        spr.bitmap = RPG::Cache.load_bitmap("Graphics/Menus/Minimap/", "#{$game_fasttravel.zone}_#{item}")
        spr.zoom_x = spr.zoom_y = @scale_multiplier
        spr.x = Graphics.width / 2 + zone.locations[item].x * @scale_multiplier
        spr.y = Graphics.height / 2 + zone.locations[item].y * @scale_multiplier
        spr.opacity = 0
        spr.shader = Shader::WorldMachine
        @data_locations[item] = spr
        set_color(item)
      else
        #Legacy
        spr = Sprite.new(@viewport)
        spr.bitmap = Bitmap.new(320, ITEM_SPACING)
        spr.x = MARGIN
        spr.y = TITLE_MARGIN + TITLE_TOP_MARGIN + ITEM_SPACING * i + 32
        spr.opacity = 0
        spr.bitmap.draw_text(0, 0, spr.bitmap.width, spr.bitmap.height, zone.maps[item])
        @data_sprites << spr
      end
    end
    
    if (Settings[:fasttravel_ui] == 0)
      return
    end
    
    #WME
    if @data_locations.has_key?(@selected_location)
      @niko_pos_x = (zone.locations[@selected_location].x.to_f + zone.locations[@selected_location].niko_x.to_f)
      @niko_pos_y = (zone.locations[@selected_location].y.to_f + zone.locations[@selected_location].niko_y.to_f)

      @niko_icon.x = @niko_pos_x.to_i * @scale_multiplier + Graphics.width / 2
      @niko_icon.y = @niko_pos_y.to_i * @scale_multiplier + Graphics.height / 2
    end

    update_location_name_text
    update_arrows_positions
  end

  def update
    zone = ZONES[$game_fasttravel.zone]
    zone = zone == nil ? Zone.new("", {}, {}) : zone
    if zone.locations != nil && !(Settings[:fasttravel_ui] == 0)
      @arrows_timer += 1
      if @arrows_timer >= 30
        @arrows_timer = 0
        @arrows_offset = @arrows_offset == 1 ? 0 : 1
      end
    end

    if @fade_in
      self.opacity += 20

      if zone.locations != nil && !(Settings[:fasttravel_ui] == 0)
        #WME
        @niko_icon.opacity += 20
        @location_name_text.opacity += 20
        
        location = zone.locations[@selected_location]
        @arrow_top.opacity += @data.include?(location.next_top) ? 20 : 0
        @arrow_bottom.opacity += @data.include?(location.next_bottom) ? 20 : 0
        @arrow_left.opacity += @data.include?(location.next_left) ? 20 : 0
        @arrow_right.opacity += @data.include?(location.next_right) ? 20 : 0
      end

      active_spr = nil
      @data_sprites.each do |spr|
        spr.opacity += 10
        spr.opacity = 128 if spr.opacity > 128
        active_spr = spr if !active_spr && spr.x < MARGIN * 2
      end
      #WME
      if !(Settings[:fasttravel_ui] == 0)
        @data_locations.each do |name, spr|
          spr.opacity += 20
          spr.opacity = 255 if spr.opacity > 255
        end
      end
      #Not WME
      if active_spr
        active_spr.x += 6
        active_spr.x = MARGIN * 2 if active_spr.x > MARGIN * 2
      elsif self.opacity == 255
        @fade_in = false
      end
      return
    end

    if @fade_out
      self.opacity -= 20

      if zone.locations != nil && !(Settings[:fasttravel_ui] == 0)
        #WME
        @niko_icon.opacity -= 20
        @location_name_text.opacity -= 20
        
        location = zone.locations[@selected_location]
        @arrow_top.opacity -= @data.include?(location.next_top) ? 20 : 0
        @arrow_bottom.opacity -= @data.include?(location.next_bottom) ? 20 : 0
        @arrow_left.opacity -= @data.include?(location.next_left) ? 20 : 0
        @arrow_right.opacity -= @data.include?(location.next_right) ? 20 : 0
      end

      @data_sprites.each do |spr|
        spr.opacity -= 10
      end
      
      #WME
      if !(Settings[:fasttravel_ui] == 0)
        @data_locations.each do |name, spr|
          spr.opacity -= 20
        end
      end

      #Not WME
      if self.opacity == 0
        @fade_out = false
        self.visible = false
        @data_sprites.each do |spr|
          spr.dispose
        end
        
        #WME
        if !(Settings[:fasttravel_ui] == 0)
          @data_locations.each do |name, spr|
            spr.dispose
          end
          @data_locations = {}
        end

        #Not WME
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

    #WME noik
    if zone.locations != nil && !(Settings[:fasttravel_ui] == 0)
      @niko_target_pos_x = (zone.locations[@selected_location].x.to_f + zone.locations[@selected_location].niko_x.to_f)
      @niko_target_pos_y = (zone.locations[@selected_location].y.to_f + zone.locations[@selected_location].niko_y.to_f)

      if @data_locations.has_key?(@selected_location)
        @niko_pos_x = (@niko_target_pos_x * 0.25 + @niko_pos_x * 0.75)
        @niko_pos_y = (@niko_target_pos_y * 0.25 + @niko_pos_y * 0.75)
      end

      @niko_pos_x = (@niko_pos_x - @niko_target_pos_x).abs < 0.5 ? @niko_target_pos_x : @niko_pos_x
      @niko_pos_y = (@niko_pos_y - @niko_target_pos_y).abs < 0.5 ? @niko_target_pos_y : @niko_pos_y

      @niko_icon.x = @niko_pos_x.to_i * @scale_multiplier + Graphics.width / 2
      @niko_icon.y = @niko_pos_y.to_i * @scale_multiplier + Graphics.height / 2
      
      update_arrows_positions
      
      location = zone.locations[@selected_location]
      @arrow_top.opacity += @data.include?(location.next_top) ? 40 : -40
      @arrow_bottom.opacity += @data.include?(location.next_bottom) ? 40 : -40
      @arrow_left.opacity += @data.include?(location.next_left) ? 40 : -40
      @arrow_right.opacity += @data.include?(location.next_right) ? 40 : -40
    elsif !(Settings[:fasttravel_ui] == 0)
      @niko_icon.opacity = 
      @arrow_top.opacity = @arrow_bottom.opacity =
      @arrow_left.opacity = @arrow_right.opacity = 0
    end

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

    if !(Settings[:fasttravel_ui] == 0)
      #WME
      @data_locations.each do |name, spr|
          set_color(name)
      end
    end

    if Input.trigger?(Input::UP)
      if zone.locations != nil && !(Settings[:fasttravel_ui] == 0)
        next_location = zone.locations[@selected_location].next_top
        if @data_locations.has_key?(next_location)
          @selected_location = next_location
          $game_system.se_play($data_system.cursor_se)
          update_location_name_text
        end
      else
        @index = (@index - 1) % @data.size
        $game_system.se_play($data_system.cursor_se)
      end
    end
    if Input.trigger?(Input::DOWN)
      if zone.locations != nil && !(Settings[:fasttravel_ui] == 0)
        next_location = zone.locations[@selected_location].next_bottom
        if @data_locations.has_key?(next_location)
          @selected_location = next_location
          $game_system.se_play($data_system.cursor_se)
          update_location_name_text
        end
      else
        @index = (@index + 1) % @data.size
        $game_system.se_play($data_system.cursor_se)
      end
    end
    
    if Input.trigger?(Input::LEFT)
      if zone.locations != nil && !(Settings[:fasttravel_ui] == 0)
        next_location = zone.locations[@selected_location].next_left
        if @data_locations.has_key?(next_location)
          @selected_location = next_location
          $game_system.se_play($data_system.cursor_se)
          update_location_name_text
        end
      end
    end
    if Input.trigger?(Input::RIGHT)
      if zone.locations != nil && !(Settings[:fasttravel_ui] == 0)
        next_location = zone.locations[@selected_location].next_right
        if @data_locations.has_key?(next_location)
          @selected_location = next_location
          $game_system.se_play($data_system.cursor_se)
          update_location_name_text
        end
      end
    end

    if Input.trigger?(Input::ACTION)
      $game_system.se_play($data_system.decision_se)
      choice = $game_fasttravel.unlocked_maps[@selected_location]
      if zone.locations == nil || (Settings[:fasttravel_ui] == 0)
        choice = $game_fasttravel.unlocked_maps[@data[@index]]
      end
      if choice.id != $game_map.map_id
        @transfer_player = choice
      end
      @fade_out = true
      return
    end

    if Input.trigger?(Input::CANCEL)
      $game_system.se_play($data_system.cancel_se)
      @fade_out = true
    end
  end

  # Attributes
  def visible
    @viewport.visible
  end
  def visible=(val)
    @viewport.visible = val
  end
  def opacity=(val)
    @bg.opacity = val
    @title.opacity = val
  end
  def opacity
    @bg.opacity
  end
end
