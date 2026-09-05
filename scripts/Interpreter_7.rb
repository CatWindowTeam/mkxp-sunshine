#==============================================================================
# ** Interpreter (part 7)
#------------------------------------------------------------------------------
#  This interpreter runs event commands. This class is used within the
#  Game_System class and the Game_Event class.
#==============================================================================

class Interpreter
  #--------------------------------------------------------------------------
  # * Change Enemy HP
  #--------------------------------------------------------------------------
  def command_331
    # Get operate value
    value = operate_value(@parameters[1], @parameters[2], @parameters[3])
    # Process with iterator
    iterate_enemy(@parameters[0]) do |enemy|
      # If HP is not 0
      if enemy.hp > 0
        # Change HP (if death is not permitted then change HP to 1)
        if @parameters[4] == false and enemy.hp + value <= 0
          enemy.hp = 1
        else
          enemy.hp += value
        end
      end
    end
    # Continue
    true
  end
  #--------------------------------------------------------------------------
  # * Change Enemy SP
  #--------------------------------------------------------------------------
  def command_332
    # Get operate value
    value = operate_value(@parameters[1], @parameters[2], @parameters[3])
    # Process with iterator
    iterate_enemy(@parameters[0]) do |enemy|
      # Change SP
      enemy.sp += value
    end
    # Continue
    true
  end
  #--------------------------------------------------------------------------
  # * Change Enemy State
  #--------------------------------------------------------------------------
  def command_333
    # Process with iterator
    iterate_enemy(@parameters[0]) do |enemy|
      # If [regard HP 0] state option is valid
      if $data_states[@parameters[2]].zero_hp
        # Clear immortal flag
        enemy.immortal = false
      end
      # Change
      if @parameters[1] == 0
        enemy.add_state(@parameters[2])
      else
        enemy.remove_state(@parameters[2])
      end
    end
    # Continue
    true
  end
  #--------------------------------------------------------------------------
  # * Enemy Recover All
  #--------------------------------------------------------------------------
  def command_334
    # Process with iterator
    iterate_enemy(@parameters[0]) do |enemy|
      # Recover all
      enemy.recover_all
    end
    # Continue
    true
  end
  #--------------------------------------------------------------------------
  # * Enemy Appearance
  #--------------------------------------------------------------------------
  def command_335
    # Get enemy
    enemy = $game_troop.enemies[@parameters[0]]
    # Clear hidden flag
    if enemy != nil
      enemy.hidden = false
    end
    # Continue
    true
  end
  #--------------------------------------------------------------------------
  # * Enemy Transform
  #--------------------------------------------------------------------------
  def command_336
    # Get enemy
    enemy = $game_troop.enemies[@parameters[0]]
    # Transform processing
    if enemy != nil
      enemy.transform(@parameters[1])
    end
    # Continue
    true
  end
  #--------------------------------------------------------------------------
  # * Show Battle Animation
  #--------------------------------------------------------------------------
  def command_337
    # Process with iterator
    iterate_battler(@parameters[0], @parameters[1]) do |battler|
      # If battler exists
      if battler.exist?
        # Set animation ID
        battler.animation_id = @parameters[2]
      end
    end
    # Continue
    true
  end
  #--------------------------------------------------------------------------
  # * Deal Damage
  #--------------------------------------------------------------------------
  def command_338
    # Get operate value
    value = operate_value(0, @parameters[2], @parameters[3])
    # Process with iterator
    iterate_battler(@parameters[0], @parameters[1]) do |battler|
      # If battler exists
      if battler.exist?
        # Change HP
        battler.hp -= value
        # If in battle
        if $game_temp.in_battle
          # Set damage
          battler.damage = value
          battler.damage_pop = true
        end
      end
    end
    # Continue
    true
  end
  #--------------------------------------------------------------------------
  # * Force Action
  #--------------------------------------------------------------------------
  def command_339
    # Ignore if not in battle
    unless $game_temp.in_battle
      return true
    end
    # Ignore if number of turns = 0
    if $game_temp.battle_turn == 0
      return true
    end
    # Process with iterator (For convenience, this process won't be repeated)
    iterate_battler(@parameters[0], @parameters[1]) do |battler|
      # If battler exists
      if battler.exist?
        # Set action
        battler.current_action.kind = @parameters[2]
        if battler.current_action.kind == 0
          battler.current_action.basic = @parameters[3]
        else
          battler.current_action.skill_id = @parameters[3]
        end
        # Set action target
        if @parameters[4] >= 0
          battler.current_action.target_index = @parameters[4]
        end
        # Set force flag
        battler.current_action.forcing = true
        # If action is valid and [run now]
        if battler.current_action.valid? and @parameters[5] == 1
          # Set battler being forced into action
          $game_temp.forcing_battler = battler
          # Advance index
          @index += 1
          # End
          return false
        end
      end
    end
    # Continue
    true
  end
  #--------------------------------------------------------------------------
  # * Abort Battle
  #--------------------------------------------------------------------------
  def command_340
    # Set battle abort flag
    $game_temp.battle_abort = true
    # Advance index
    @index += 1
    # End
    false
  end
  #--------------------------------------------------------------------------
  # * Call Menu Screen
  #--------------------------------------------------------------------------
  def command_351
    # Set battle abort flag
    $game_temp.battle_abort = true
    # Set menu calling flag
    $game_temp.menu_calling = true
    # Advance index
    @index += 1
    # End
    false
  end
  #--------------------------------------------------------------------------
  # * Call Save Screen
  #--------------------------------------------------------------------------
  def command_352
    # Set battle abort flag
    $game_temp.battle_abort = true
    # Set save calling flag
    $game_temp.save_calling = true
    # Advance index
    @index += 1
    # End
    false
  end
  #--------------------------------------------------------------------------
  # * Game Over
  #--------------------------------------------------------------------------
  def command_353
    # Set game over flag
    $game_temp.gameover = true
    # End
    false
  end
  #--------------------------------------------------------------------------
  # * Return to Title Screen
  #--------------------------------------------------------------------------
  def command_354
    # Set return to title screen flag
    $game_temp.to_title = true
    # End
    false
  end
  #--------------------------------------------------------------------------
  # * Script
  #--------------------------------------------------------------------------
  def command_355
    # Set first line to script
    script = @list[@index].parameters[0] + "\n"
    # Loop
    while true
      # If next event command is second line of script or after
      if @list[@index+1].code == 655 || @list[@index+1].code == 355
        # Add second line or after to script
        script += @list[@index+1].parameters[0] + "\n"
      # If event command is not second line or after
      else
        # Abort loop
        break
      end
      # Advance index
      @index += 1
    end
    # Continue
    begin
      eval(script, binding, "event")
    rescue => e
      event = get_character(0)
      event_info = "Event ID:#{event&.id || "???"} #{event&.name || "Unknown Event"}"
      error_line = e.backtrace.find { |line| line.include?("event") }
      line_number = error_line&.split(":")[1]
      
      formated_script = ""
      script.each_line.with_index(1) do |line, index|
        if line_number.to_i == index
          formated_script << "%3d > %s\n" % [index, line.chomp]
        else
          formated_script << "%3d | %s\n" % [index, line.chomp]
        end
      end
      result = "\n[EVENT] Failed to execute event script | " + event_info
      result << "\n------------------------------------------\n"
      result << formated_script
      result << "------------------------------------------\n"
      result << e.message
      result << "\n----------------Backtrace-----------------\n"
      e.backtrace.each do |line|
        result << "#{line}\n"
      end
      result << "------------------------------------------\n"
      puts result
    end
    true
  end
end
