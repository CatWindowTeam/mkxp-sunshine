class DynamicLight
  def initialize(viewport)
    @light_sprite = LightMap.new(viewport)
    #@light_sprite.wallmap = Bitmap.new($game_map.width, $game_map.height)
    
    #for x in 0...$game_map.width
    #  for y in 0...$game_map.height
    #    @light_sprite.wallmap.set_pixel(x, y, Color.new(
    #      b(light_passable($game_map.data[x, y, 0])),
    #      b(light_passable($game_map.data[x, y, 1])),
    #      b(light_passable($game_map.data[x, y, 2]))
    #    ))
    #  end
    #end
    
    if $light == nil
      return
    end

    @light_sprite.ambient = $light.ambient_light
    $light.static_lights.each do |source|
      @light_sprite.add_static_source(
        source[0],
        source[1],
        source[2],
        source[3],
        source[4]
      )
    end
  end

  def dispose
    @light_sprite.dispose
  end

  def update
    # dont render light if its disabled or not awaked
    @light_sprite.visible = Settings[:light] && $light.awaked
    if (!Settings[:light] && $light.awaked)
      return
    end

    if $light.update_clear_lights
      @light_sprite.clear_static_sources()
      @light_sprite.clear_dynamic_sources()
      $light.update_clear_lights = false
    end
    if $light.update_ambient_light
      @light_sprite.ambient = $light.ambient_light
      $light.update_ambient_light = false
    end
    if $light.update_remove_static_light.length > 0
      $light.update_remove_static_light.each do |source|
        @light_sprite.remove_static_source(source[0], source[1])
      end
      $light.update_remove_static_light = []
    end
    if $light.update_static_lights.length > 0
      $light.update_static_lights.each do |source|
        @light_sprite.add_static_source(
          source[0],
          source[1],
          source[2],
          source[3],
          source[4]
        )
      end
      $light.update_static_lights = []
    end

    # shader uniforms
    @light_sprite.camera_x = $game_map.display_x / 4
    @light_sprite.camera_y = $game_map.display_y / 4
    @light_sprite.tilemap_offset_x = ($game_map.display_x / 128) * 32 - $game_map.display_x / 4
    @light_sprite.tilemap_offset_y = ($game_map.display_y / 128) * 32 - $game_map.display_y / 4

    # clear all npc light
    @light_sprite.clear_dynamic_sources()
    
    # player light
    table_offset = ($game_player.direction - 2) * 2
    offset_x = $light.plr_light_table[table_offset]
    offset_y = $light.plr_light_table[table_offset + 1]
    power_mult = $light.plr_light_table[table_offset + 2]
    plr_light = [
      $game_player.real_x / 128.0 + offset_x,
      $game_player.real_y / 128.0 + offset_y,
      $light.plr_light_power * power_mult,
      $light.plr_light_radius_lerped,
      $light.plr_light_color
    ]
    
    if has_effect(plr_light)
      @light_sprite.add_dynamic_source(plr_light[0], plr_light[1], plr_light[2], plr_light[3], plr_light[4])
    end

    # add npc light
    $light.dynamic_lights.each { |npc_light_source|
      npc_light = [$game_map.events[npc_light_source[0]].real_x / 128.0, $game_map.events[npc_light_source[0]].real_y / 128.0, npc_light_source[1], npc_light_source[2], npc_light_source[3]]
      if has_effect(npc_light)
        @light_sprite.add_dynamic_source(npc_light[0], npc_light[1] - 0.5, npc_light[2], npc_light[3], npc_light[4])
      end
    }
  end

  #def light_passable(tile_id)
  #  $game_map.passages[tile_id] & 15 == 0
  #end

  #def b(val)
  #  val ? 255 : 0
  #end

  def has_effect(source)
    source[2] != 0 && source[3] > 0 && source[4].alpha >= 0 && (source[4].red != 0 || source[4].green != 0 || source[4].blue != 0)
  end
end
