class DynamicLight
  attr_accessor :done
  attr_accessor :plr_light_power
  attr_accessor :plr_light_color

  def initialize(viewport)
    @done = false # for events
    @awaked = false

    @light_npc = []

    @ambient_light = 255
    @ambient_light_lerped = 255

    @plr_light_power = 0
    @plr_light_radius = -1.0
    @plr_light_radius_lerped = 5.0
    @plr_light_color = Color.new(255, 255, 255)
    @plr_light_table = [
    # offset_x, offset_y, power_mult, radius mult
        0.0, -0.5, 1.0, 1.0, # down
      -0.25, -0.5, 1.0, 0.7, # left
       0.25, -0.5, 1.0, 0.7, # right
        0.0,  0.0, 0.0, 1.0  # up
    ]

    @light_sprite = LightMap.new(viewport)
    @light_sprite.wallmap = Bitmap.new($game_map.width, $game_map.height)
    
    for x in 0...$game_map.width
      for y in 0...$game_map.height
        @light_sprite.wallmap.set_pixel(x, y, Color.new(
          b(light_passable($game_map.data[x, y, 0])),
          b(light_passable($game_map.data[x, y, 1])),
          b(light_passable($game_map.data[x, y, 2]))
        ))
      end
    end
  end

  def dispose
    @light_sprite.dispose
  end

  def update
    # dont render light if its disabled or not awaked
    @light_sprite.visible = Settings[:light] && @awaked
    if (!Settings[:light] && @awaked)
      return
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
    offset_x = @plr_light_table[table_offset]
    offset_y = @plr_light_table[table_offset + 1]
    power_mult = @plr_light_table[table_offset + 2]
    power_radius = @plr_light_table[table_offset + 3]
    @plr_light_radius_lerped = @plr_light_radius_lerped * 0.8 + @plr_light_radius * 0.2 * power_radius
    plr_light = [
      $game_player.real_x / 128.0 + offset_x,
      $game_player.real_y / 128.0 + offset_y,
      @plr_light_power * power_mult,
      @plr_light_radius_lerped,
      @plr_light_color
    ]
    
    if has_effect(plr_light)
      @light_sprite.add_dynamic_source(plr_light[0], plr_light[1], plr_light[2], plr_light[3], plr_light[4])
    end

    # add npc light
    @light_npc.each { |npc_light_source|
      npc_light = [npc_light_source[0].real_x / 128.0, npc_light_source[0].real_y / 128.0, npc_light_source[1], npc_light_source[2], npc_light_source[3]]
      if has_effect(npc_light)
        @light_sprite.add_dynamic_source(npc_light[0], npc_light[1] - 0.5, npc_light[2], npc_light[3], npc_light[4])
      end
    }
  end

  def light_passable(tile_id)
    $game_map.passages[tile_id] & 15 == 0
  end

  def b(val)
    val ? 255 : 0
  end
  
  def add(x, y, power, radius, color = Color.new(255, 255, 255))
    @awaked = true
    @light_sprite.add_static_source(x, y, power, radius, color)
  end
    
  def add_npc(npc_id, power, radius, color = Color.new(255, 255, 255))
    @awaked = true
    @light_npc << [$game_map.events[npc_id], power, radius, color]
  end

  def has_effect(source)
    source[2] != 0 && source[3] > 0 && source[4].alpha >= 0 && (source[4].red != 0 || source[4].green != 0 || source[4].blue != 0)
  end

  #def remove(x, y)
  #  @light_sources.each do |sourse|
  #    if sourse[0] == x && sourse[1] == y
  #      @light_sources.delete(sourse)
  #      return true
  #    end
  #  end
  #  return false
  #end
  
  def awake
    @awaked = true
  end
  
  def sleep # fully disables dynamic light system
    @awaked = false
  end
  
  def ambient_light
    @ambient_light
  end
  def ambient_light=(val)
    @ambient_light = val
    @light_sprite.ambient = val
    @awaked = true
  end

  def plr_light_radius
    @plr_light_radius
  end
  def plr_light_radius=(val)
    if @plr_light_radius < 0
      @plr_light_radius_lerped = val
    end
    @plr_light_radius = val
  end

  def plr_light_table
    @plr_light_table
  end
  def plr_light_table=(val)
    puts "aboba"
    @plr_light_table[0]  = val&.[](0)  || 0
    @plr_light_table[1]  = val&.[](1)  || 0
    @plr_light_table[2]  = val&.[](2)  || 1
    @plr_light_table[3]  = val&.[](3)  || 1
    @plr_light_table[4]  = val&.[](4)  || 0
    @plr_light_table[5]  = val&.[](5)  || 0
    @plr_light_table[6]  = val&.[](6)  || 1
    @plr_light_table[7]  = val&.[](7)  || 1
    @plr_light_table[8]  = val&.[](8)  || 0
    @plr_light_table[9]  = val&.[](9)  || 0
    @plr_light_table[10] = val&.[](10) || 1
    @plr_light_table[11] = val&.[](11) || 1
    @plr_light_table[12] = val&.[](12) || 0
    @plr_light_table[13] = val&.[](13) || 0
    @plr_light_table[14] = val&.[](14) || 1
    @plr_light_table[15] = val&.[](15) || 1
    puts "aboba2"
  end
end