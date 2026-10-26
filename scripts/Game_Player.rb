#==============================================================================
# ** Game_Player
#------------------------------------------------------------------------------
#  This class handles the player. Its functions include event starting
#  determinants and map scrolling. Refer to "$game_player" for the one
#  instance of this class.
#==============================================================================

class Game_Player < Game_Character
  attr_reader :move_speed

  def initialize
    @center_x = ((Graphics.width / 2) - 16) * 4   # Center screen x-coordinate * 4
    @center_y = ((Graphics.height / 2) - 16) * 4   # Center screen y-coordinate * 4
    @event_move_mode = false
    super
  end
  #--------------------------------------------------------------------------
  # * Passable Determinants
  #     x : x-coordinate
  #     y : y-coordinate
  #     d : direction (0,2,4,6,8)
  #         * 0 = Determines if all directions are impassable (for jumping)
  #--------------------------------------------------------------------------
  def passable?(x, y, d)
    if Settings[:debug] && Input.press?(Input::DEBUGACTION)
      return true
    end
    # Get new coordinates
    new_x = x + (d == 6 ? 1 : d == 4 ? -1 : 0)
    new_y = y + (d == 2 ? 1 : d == 8 ? -1 : 0)
    # If coordinates are outside of map
    unless $game_map.valid?(new_x, new_y)
      # Impassable
      return false
    end
    super
  end
  #--------------------------------------------------------------------------
  # * Set Map Display Position to Center of Screen
  #--------------------------------------------------------------------------
  def center(x, y)
  	max_x = ($game_map.width - (Graphics.width / 32.0).round) * 128
    max_y = ($game_map.height - (Graphics.height / 32.0).round) * 128
    $game_map.display_x = [0, [x * 128 - @center_x, max_x].min].max
    $game_map.display_y = [0, [y * 128 - @center_y, max_y].min].max
  end
  #--------------------------------------------------------------------------
  # * Move to Designated Position
  #     x : x-coordinate
  #     y : y-coordinate
  #--------------------------------------------------------------------------
  def moveto(x, y)
    super
    # Centering
    center(x, y)
  end
  #--------------------------------------------------------------------------
  # * Increaase Steps
  #--------------------------------------------------------------------------
  def increase_steps
    super
    # If move route is not forcing
    unless @move_route_forcing
      # Increase steps
      $game_party.increase_steps
      # Number of steps are an even number
      if $game_party.steps % 2 == 0
        # Slip damage check
        $game_party.check_map_slip_damage
      end
    end
  end
  #--------------------------------------------------------------------------
  # * Refresh
  #--------------------------------------------------------------------------
  def refresh
    # If party members = 0
    if $game_party.actors.size == 0
      # Clear character file name and hue
      @character_name = ""
      @character_hue = 0
      # End method
      return
    end
    # Get lead actor
    actor = $game_party.actors[0]
    # Set character file name and hue
    @character_name = actor.character_name
    @character_hue = actor.character_hue
    # Initialize opacity level and blending method
    @opacity = 255
    @blend_type = 0
  end
  #--------------------------------------------------------------------------
  # * Same Position Starting Determinant
  #--------------------------------------------------------------------------
  def check_event_trigger_here(triggers)
    result = false
    # If event is running
    if $game_system.map_interpreter.running?
      return result
    end
    # All event loops
    $game_map.events.values.each do |event|
      # If event coordinates and triggers are consistent
      if event.x == @x and event.y == @y and triggers.include?(event.trigger)
        # If starting determinant is same position event (other than jumping)
        if not event.jumping? and event.over_trigger?
          event.start
          result = true
        end
      end
    end
    return result
  end
  #--------------------------------------------------------------------------
  # * Front Envent Starting Determinant
  #--------------------------------------------------------------------------
  def check_event_trigger_there(triggers)
    result = false
    # If event is running
    if $game_system.map_interpreter.running?
      return result
    end
    # Calculate front event coordinates
    new_x = @x + (@direction == 6 ? 1 : @direction == 4 ? -1 : 0)
    new_y = @y + (@direction == 2 ? 1 : @direction == 8 ? -1 : 0)
    # All event loops
    $game_map.events.values.each do |event|
      # If event coordinates and triggers are consistent
      if event.intersects?(new_x, new_y) && triggers.include?(event.trigger)
        # If starting determinant is front event (other than jumping)
        if not event.jumping? and not event.over_trigger?
          event.start
          result = true
        end
      end
    end
    return result
  end
  #--------------------------------------------------------------------------
  # * Touch Event Starting Determinant
  #--------------------------------------------------------------------------
  def check_event_trigger_touch(x, y)
    result = false
    # If event is running
    if $game_system.map_interpreter.running?
      return result
    end
    # All event loops
    $game_map.events.values.each do |event|
      # If event coordinates and triggers are consistent
      if event.x == x and event.y == y and [1,2].include?(event.trigger)
        # If starting determinant is front event (other than jumping)
        if not event.jumping? and not event.over_trigger?
          event.start
          result = true
        end
      end
    end
    return result
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    if (Settings[:debug] && Input::key_press?(Input::key_from_name("F6")))
      unlock
    end
    old_center_x = @center_x
    old_center_y = @center_y
    @center_x = ((Graphics.width / 2) - 16) * 4   # Center screen x-coordinate * 4
    @center_y = ((Graphics.height / 2) - 16) * 4   # Center screen y-coordinate * 4
    if (@center_x != old_center_x)
      $game_map.display_x = @real_x.to_i - @center_x
    end
    if (@center_y != old_center_y)
      $game_map.display_y = @real_y.to_i - @center_y
    end
    # Remember whether or not moving in local variables
    last_moving = moving?
    # Remember coordinates in local variables
    last_real_x = @real_x
    last_real_y = @real_y
    # If moving, event running, move route forcing, and message window
    # display are all not occurring
    unless $game_system.map_interpreter.running? || @move_route_forcing ||
           $game_temp.message_window_showing || $game_temp.menus_visible
      # Adjust move speed
      if !$game_switches[251]
        @move_speed = Input.press?(Input::RUN) ? 4 : 3
      else
        @move_speed = Input.press?(Input::RUN) ? 3 : 4
      end
      if !@event_move_mode && !moving?
        # Move player in the direction the directional button is being pressed
        case Input.dir4
        when 2
          turn_down
        when 4
          turn_left
        when 6
          turn_right
        when 8
          turn_up
        end
        distance = 2 ** @move_speed * Graphics.scaled_delta
        x = (Input::press?(Input::RIGHT) ? 0.0 : -1.0) + (Input::press?(Input::LEFT) ? 0.0 : 1.0)
        y = (Input::press?(Input::DOWN) ? 0.0 : -1.0) + (Input::press?(Input::UP) ? 0.0 : 1.0)
        if (x.abs + y.abs != 0)
          length = Math.sqrt(x * x + y * y)
          smooth_move(x / length * distance, y / length * distance)
        else
          straighten
        end
        # moving from obstacle if it is too near
        if !passable?(@x, @y, 8) && @y > ((@real_y + 64).to_i / 128)
          # Move down
          @real_y = @real_y + distance
        end
        if !passable?(@x, @y, 6) && @x < ((@real_x + 64).to_i / 128)
          # Move left
          @real_x = @real_x - distance
        end
        if !passable?(@x, @y, 4) && @x > ((@real_x + 32).to_i / 128)
          # Move right
          @real_x = @real_x + distance
        end
        if !passable?(@x, @y, 2) && @y < ((@real_y + 128).to_i / 128)
          # Move up
          @real_y = @real_y - distance
        end
      else
        @event_move_mode = false if !moving?
      end
    else
      @move_speed = 2
    end
    super
    # Emit footsteps
    if moving? || last_real_x != @real_x || last_real_y != @real_y
      @footstep_timer += 1 * Graphics.scaled_delta
      if (@move_speed <= 3 && @footstep_timer >= 16) || (@move_speed >= 4 && @footstep_timer >= 12)
        emit_footstep
        @footstep_timer = 0
      end
    else
      @footstep_timer = 8
    end
    if !$game_switches[100]
      # If character moves down and is positioned lower than the center
      # of the screen
      if @real_y > last_real_y and @real_y - $game_map.display_y > @center_y
        # Scroll map down
        $game_map.scroll_down(@real_y - last_real_y)
      end
      # If character moves left and is positioned more let on-screen than
      # center
      if @real_x < last_real_x and @real_x - $game_map.display_x < @center_x
        # Scroll map left
        $game_map.scroll_left(last_real_x - @real_x)
      end
      # If character moves right and is positioned more right on-screen than
      # center
      if @real_x > last_real_x and @real_x - $game_map.display_x > @center_x
        # Scroll map right
        $game_map.scroll_right(@real_x - last_real_x)
      end
      # If character moves up and is positioned higher than the center
      # of the screen
      if @real_y < last_real_y and @real_y - $game_map.display_y < @center_y
        # Scroll map up
        $game_map.scroll_up(last_real_y - @real_y)
      end
    end
    # If not moving
    unless moving? or $game_temp.menus_visible
      # If player was moving last time
      if last_moving
        # Event determinant is via touch of same position event
        check_event_trigger_here([1,2])
      end
      # If C button was pressed
      if Input.trigger?(Input::ACTION)
        # Same position and front event determinant
        check_event_trigger_here([0])
        check_event_trigger_there([0,1,2])
      end
    end
  end

  # Footsteps
  def emit_footstep
    return unless $game_temp.footstep_sfx
    tag = $game_map.terrain_tag(@x, @y) - 1
    if tag >= 0 && tag < $game_temp.footstep_sfx.size 
      name = $game_temp.footstep_sfx[tag]
      if name.kind_of?(Array)
        name, volume = name
      else
        volume = 1.0
      end
      if FOOTSTEP_AMT.include? name
        name += '%02d' % [rand(FOOTSTEP_AMT[name]) + 1]
      end
      pitch = 85 + rand(30)
      vol = 70 + rand(20)
      if $game_switches[112] == true
        name = "wheel_squeak1"
        pitch = 120 + rand(10)
        if @wheel_squeak != true
          pitch += 10
          @wheel_squeak = true
        else
          @wheel_squeak = false
        end
      end
      Audio.se_play("Audio/SE/#{name}", vol * volume, pitch)
    end
    emit_footsplash(@direction)
  end

  def update_stop
    if !@event_move_mode
      return
    end
    super
  end

  def smooth_move(x = 0, y = 0)
    next_real_x = @real_x + x
    next_real_y = @real_y + y
    next_x = ((next_real_x + 64 + (x <=> 0) * 32) / 128).to_i
    next_y = ((next_real_y + 96 + (y <=> 0) * 32) / 128).to_i
    @real_x += x if @x == next_x || passable?(@x, @y, x > 0 ? 6 : 4)
    @real_y += y if @y == next_y || passable?(@x, @y, y > 0 ? 2 : 8)
    if @x != next_x || @y != next_y
      increase_steps
      check_event_trigger_touch(next_x, next_y)
    end
    if (@real_x == next_real_x && x != 0) || (@real_y == next_real_y && y != 0)
      if @walk_anime
        # Increase animation count by 1.5
        @anime_count += 1.5 * Graphics.scaled_delta
      # If move animation is OFF, and stop animation is ON
      elsif @step_anime
        # Increase animation count by 1
        @anime_count += 1 * Graphics.scaled_delta
      end
    else
    end
    @x = ((@real_x + 64) / 128).to_i
    @y = ((@real_y + 96) / 128).to_i
    check_event_trigger_here([1, 2])
  end

  def update_move
    # Convert map coordinates from map move speed into move distance
    # If move animation is ON
    distance = 2 ** @move_speed * Graphics.scaled_delta
    if @event_move_mode
      # If logical coordinates are further down than real coordinates
      if @y * 128 > @real_y
        # Move down
        @real_y = [@real_y + distance, @y * 128].min
      end
      # If logical coordinates are more to the left than real coordinates
      if @x * 128 < @real_x
        # Move left
        @real_x = [@real_x - distance, @x * 128].max
      end
      # If logical coordinates are more to the right than real coordinates
      if @x * 128 > @real_x
        # Move right
        @real_x = [@real_x + distance, @x * 128].min
      end
      # If logical coordinates are further up than real coordinates
      if @y * 128 < @real_y
        # Move up
        @real_y = [@real_y - distance, @y * 128].max
      end
    end
    if @walk_anime
      # Increase animation count by 1.5
      @anime_count += 1.5 * Graphics.scaled_delta
    # If move animation is OFF, and stop animation is ON
    elsif @step_anime
      # Increase animation count by 1
      @anime_count += 1 * Graphics.scaled_delta
    end
    # Wrap position
    if @x * 128 == @real_x.to_i && @y * 128 == @real_y.to_i
      @x %= $game_map.width
      @y %= $game_map.height
      @real_x = @x * 128
      @real_y = @y * 128
    end
  end

  def moving?
    return @event_move_mode && super
  end

  def move_down(turn_enabled = true)
    @event_move_mode = true
    super
  end
  
  def move_left(turn_enabled = true)
    @event_move_mode = true
    super
  end
  
  def move_right(turn_enabled = true)
    @event_move_mode = true
    super
  end
  
  def move_up(turn_enabled = true)
    @event_move_mode = true
    super
  end
  
  def move_lower_left
    @event_move_mode = true
    super
  end
  
  def move_lower_right
    @event_move_mode = true
    super
  end
  
  def move_upper_left
    @event_move_mode = true
    super
  end
  
  def move_upper_right
    @event_move_mode = true
    super
  end
end
