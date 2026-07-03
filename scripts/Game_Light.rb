class Game_Light
  attr_accessor :plr_light_power
  attr_accessor :plr_light_color

  attr_accessor :awaked

  attr_accessor :update_clear_lights
  attr_accessor :update_ambient_light
  attr_accessor :update_static_lights
  attr_accessor :update_remove_static_light
  attr_reader :dynamic_lights
  attr_reader :static_lights

  def initialize
    @done = false # deprecated
    @awaked = false # means is light system enabled

    @dynamic_lights = []
    @static_lights = []

    @ambient_light = 255

    @plr_light_power = 0
    @plr_light_radius = -1.0
    @plr_light_radius_lerped = -1.0
    @plr_light_color = Color.new(255, 255, 255)
    @plr_light_table = [
    # offset_x, offset_y, power_mult, radius mult
        0.0, -0.5, 1.0, 1.0, # down
      -0.25, -0.5, 1.0, 0.7, # left
       0.25, -0.5, 1.0, 0.7, # right
        0.0,  0.0, 0.0, 1.0  # up
    ]
    
    @update_clear_lights = false
    @update_ambient_light = false
    @update_remove_static_light = []
    @update_static_lights = []
  end

  def update
    table_offset = ($game_player.direction - 2) * 2
    radius_mult = @plr_light_table[table_offset + 3]
    @plr_light_radius_lerped = @plr_light_radius_lerped * 0.8 + @plr_light_radius * 0.2 * radius_mult
  end
  
  def add(x, y, power, radius, color = Color.new(255, 255, 255))
    source = [x, y, power, radius, color]
    @update_static_lights << source
    @static_lights << source
    awake
  end
    
  def add_npc(npc_id, power, radius, color = Color.new(255, 255, 255))
    @dynamic_lights << [npc_id, power, radius, color]
    awake
  end

  def remove(x, y)
    @update_remove_static_light << [x, y]
    @static_lights.each do |source|
      if source[0] == x && source[1] == y
        @static_lights.delete(source)
        return
      end
    end
  end

  def remove_npc(npc_id)
    @dynamic_lights.each do |npc|
      if npc[0] == $game_map.events[npc_id]
        @dynamic_lights.delete(npc)
        return
      end
    end
  end

  def clear_lights
    clear_lights_no_signal
    @update_clear_lights = true
  end

  def clear_lights_no_signal
    @static_lights = []
    @dynamic_lights = []
    @update_remove_static_light = []
    @update_static_lights = []
  end

  def done
    puts "variable \"done\" of $light is deprecated, please dont use it"
    @done
  end
  def done=(val)
    puts "variable \"done\" of $light is deprecated, please dont use it"
    @done = val
  end
  
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
    @update_ambient_light = true
    awake
  end

  def plr_light_radius
    @plr_light_radius
  end
  def plr_light_radius_lerped
    @plr_light_radius_lerped
  end
  def plr_light_radius=(val)
    if @plr_light_radius < 0
      @plr_light_radius_lerped = val
    end
    @plr_light_radius = val
    awake
  end

  def plr_light_table
    @plr_light_table
  end
  def plr_light_table=(val)
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
    awake
  end
end