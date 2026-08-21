class DynamicLight
  def initialize(viewport)
    @debug_sprite = Sprite.new(viewport)
    @debug_sprite.bitmap = Bitmap.new(1, 1)
    @debug_sprite.bitmap.fill_rect(0,0,1,1,Color.new(255,255,255))
    @debug_sprite.zoom_x = Graphics.width
    @debug_sprite.zoom_y = Graphics.height
    @debug_sprite.visible = false
    @debug_sprite.z = -1
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
    
    @update_connection = Graphics.viewport_resized do |w, h|
      @debug_sprite.zoom_x = w
      @debug_sprite.zoom_y = h
    end
    
    if $light == nil
      return
    end

    $light.static_lights.each do |source|
      @light_sprite.add_static_source(
        source[0],
        source[1],
        source[2],
        source[3],
        source[4]
      )
    end
    @light_sprite.ambient = $light.ambient_light
  end

  def dispose
    @update_connection.disconnect
    @light_sprite.dispose
  end

  def update
    # dont render light if its disabled or not awaked
    @light_sprite.visible = Settings[:light] && $light.awaked
    if !Settings[:light] || !$light.awaked
      return
    end

    @debug_sprite.visible = Settings[:debug_lightmap]

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
      $game_player.screen_z + 128,
      $light.plr_light_power * power_mult,
      $light.plr_light_radius_lerped,
      $light.plr_light_color
    ]
    
    if has_effect(plr_light, $game_player.real_y)
      @light_sprite.add_dynamic_source(plr_light[0], plr_light[1], plr_light[3], plr_light[4], plr_light[5])
    end

    # add npc light
    $light.dynamic_lights.each { |npc_light_source|
      npc_light = [$game_map.events[npc_light_source[0]].real_x / 128.0, $game_map.events[npc_light_source[0]].real_y / 128.0, $game_map.events[npc_light_source[0]].screen_z + 128, npc_light_source[1], npc_light_source[2], npc_light_source[3]]
      if has_effect(npc_light, $game_map.events[npc_light_source[0]].real_y)
        @light_sprite.add_dynamic_source(npc_light[0], npc_light[1] - 0.5, npc_light[3], npc_light[4], npc_light[5])
      end
    }
  end

  #def light_passable(tile_id)
  #  $game_map.passages[tile_id] & 15 == 0
  #end

  #def b(val)
  #  val ? 255 : 0
  #end
  
  def tile_to_atlas(tile_id)
    tile_id -= 384
    y = tile_id / 8
    x = tile_id - y * 8
    [x * 32, y * 32]
  end

  def has_effect(source, char_y = 0)
    #if Settings[:light_layer_detect]
    #  #if source[4] < 3
    #    cz = (char_y - $game_map.display_y + 3) / 4 + 63
    #    x = (source[0] + 0.5).floor
    #    y = (source[1] + 0.5).floor
    #    tile0 = $game_map.get_tile(x, y, 0)
    #    tile1 = $game_map.get_tile(x, y, 1)
    #    tile2 = $game_map.get_tile(x, y, 2)
    #    tile0_priora = $game_map.priorities[tile0]
    #    tile1_priora = $game_map.priorities[tile1]
    #    tile2_priora = $game_map.priorities[tile2]
    #    tiles = [tile0, tile1, tile2]
    #    zeds = [
    #      (y * 128 - $game_map.display_y + 3) / 4 + 32 + tile0_priora * 32,
    #      (y * 128 - $game_map.display_y + 3) / 4 + 32 + tile1_priora * 32,
    #      (y * 128 - $game_map.display_y + 3) / 4 + 32 + tile2_priora * 32
    #    ]
    #    (0..2).each do |i|
    #      if zeds[i] > cz
    #        if tiles[i] > 384 # <= 384
    #        #  return false
    #        #else
    #          tileset_texture = RPG::Cache::tileset($game_map.tileset_name)
    #          tx, ty = tile_to_atlas(tiles[i])
    #          tx += (source[0] + 0.5 - x) * 32
    #          ty += (source[1] + 0.5 - y) * 32
    #          puts tx, " ", ty
    #          pixel_alpha = tileset_texture.get_pixel(tx, ty).alpha
    #          puts pixel_alpha
    #          if pixel_alpha > 127
    #            return false;
    #          end
    #        end
    #      end
    #    end
    #  #end
    #end

    source[3] != 0 && source[4] > 0 && source[5].alpha >= 0 && (source[5].red != 0 || source[5].green != 0 || source[5].blue != 0)
  end
end
