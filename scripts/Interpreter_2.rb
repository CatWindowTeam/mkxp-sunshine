#==============================================================================
# ** Interpreter (part 2)
#------------------------------------------------------------------------------
#  This interpreter runs event commands. This class is used within the
#  Game_System class and the Game_Event class.
#==============================================================================

class Interpreter
  #--------------------------------------------------------------------------
  # * Event Command Execution
  #--------------------------------------------------------------------------
  def execute_command
    # If last to arrive for list of event commands
    if @index >= @list.size - 1
      # End event
      command_end
      # Continue
      return true
    end
    # Make event command parameters available for reference via @parameters
    @parameters = @list[@index].parameters
    # Branch by command code
    case @list[@index].code
    when 101  # Show Text
      command_101
    when 102  # Show Choices
      command_102
    when 402  # When [**]
      command_402
    when 403  # When Cancel
      command_403
    when 103  # Input Number
      command_103
    when 104  # Change Text Options
      command_104
    when 105  # Button Input Processing
      command_105
    when 106  # Wait
      command_106
    when 111  # Conditional Branch
      command_111
    when 411  # Else
      command_411
    when 112  # Loop
      command_112
    when 413  # Repeat Above
      command_413
    when 113  # Break Loop
      command_113
    when 115  # Exit Event Processing
      command_115
    when 116  # Erase Event
      command_116
    when 117  # Call Common Event
      command_117
    when 118  # Label
      command_118
    when 119  # Jump to Label
      command_119
    when 121  # Control Switches
      command_121
    when 122  # Control Variables
      command_122
    when 123  # Control Self Switch
      command_123
    when 124  # Control Timer
      command_124
    when 125  # Change Gold
      command_125
    when 126  # Change Items
      command_126
    when 127  # Change Weapons
      command_127
    when 128  # Change Armor
      command_128
    when 129  # Change Party Member
      command_129
    when 131  # Change Windowskin
      command_131
    when 132  # Change Battle BGM
      command_132
    when 133  # Change Battle End ME
      command_133
    when 134  # Change Save Access
      command_134
    when 135  # Change Menu Access
      command_135
    when 136  # Change Encounter
      command_136
    when 201  # Transfer Player
      command_201
    when 202  # Set Event Location
      command_202
    when 203  # Scroll Map
      command_203
    when 204  # Change Map Settings
      command_204
    when 205  # Change Fog Color Tone
      command_205
    when 206  # Change Fog Opacity
      command_206
    when 207  # Show Animation
      command_207
    when 208  # Change Transparent Flag
      command_208
    when 209  # Set Move Route
      command_209
    when 210  # Wait for Move's Completion
      command_210
    when 221  # Prepare for Transition
      command_221
    when 222  # Execute Transition
      command_222
    when 223  # Change Screen Color Tone
      command_223
    when 224  # Screen Flash
      command_224
    when 225  # Screen Shake
      command_225
    when 231  # Show Picture
      command_231
    when 232  # Move Picture
      command_232
    when 233  # Rotate Picture
      command_233
    when 234  # Change Picture Color Tone
      command_234
    when 235  # Erase Picture
      command_235
    when 236  # Set Weather Effects
      command_236
    when 241  # Play BGM
      command_241
    when 242  # Fade Out BGM
      command_242
    when 245  # Play BGS
      command_245
    when 246  # Fade Out BGS
      command_246
    when 247  # Memorize BGM/BGS
      command_247
    when 248  # Restore BGM/BGS
      command_248
    when 249  # Play ME
      command_249
    when 250  # Play SE
      command_250
    when 251  # Stop SE
      command_251
    when 301  # Battle Processing
      command_301
    when 601  # If Win
      command_601
    when 602  # If Escape
      command_602
    when 603  # If Lose
      command_603
    when 303  # Name Input Processing
      command_303
    when 311  # Change HP
      command_311
    when 312  # Change SP
      command_312
    when 313  # Change State
      command_313
    when 314  # Recover All
      command_314
    when 315  # Change EXP
      command_315
    when 316  # Change Level
      command_316
    when 317  # Change Parameters
      command_317
    when 318  # Change Skills
      command_318
    when 319  # Change Equipment
      command_319
    when 320  # Change Actor Name
      command_320
    when 321  # Change Actor Class
      command_321
    when 322  # Change Actor Graphic
      command_322
    when 331  # Change Enemy HP
      command_331
    when 332  # Change Enemy SP
      command_332
    when 333  # Change Enemy State
      command_333
    when 334  # Enemy Recover All
      command_334
    when 335  # Enemy Appearance
      command_335
    when 336  # Enemy Transform
      command_336
    when 337  # Show Battle Animation
      command_337
    when 338  # Deal Damage
      command_338
    when 339  # Force Action
      command_339
    when 340  # Abort Battle
      command_340
    when 351  # Call Menu Screen
      command_351
    when 352  # Call Save Screen
      command_352
    when 353  # Game Over
      command_353
    when 354  # Return to Title Screen
      command_354
    when 355  # Script
      command_355
    else      # Other
      true
    end
  end
  #--------------------------------------------------------------------------
  # * End Event
  #--------------------------------------------------------------------------
  def command_end
    # Clear list of event commands
    @list = nil
    # If main map event and event ID are valid
    if @main and @event_id > 0
      # Unlock event
      $game_map.events[@event_id].unlock
    end
  end
  #--------------------------------------------------------------------------
  # * Command Skip
  #--------------------------------------------------------------------------
  def command_skip
    # Get indent
    indent = @list[@index].indent
    # Loop
    while true
      # If next event command is at the same level as indent
      if @list[@index+1].indent == indent
        # Continue
        return true
      end
      # Advance index
      @index += 1
    end
  end
  #--------------------------------------------------------------------------
  # * Get Character
  #     parameter : parameter
  #--------------------------------------------------------------------------
  def get_character(parameter)
    # Branch by parameter
    case parameter
    when -1  # player
      return $game_player
    when 0  # this event
      events = $game_map.events
      return events == nil ? nil : events[@event_id]
    else  # specific event
      events = $game_map.events
      return events == nil ? nil : events[parameter]
    end
  end
  #--------------------------------------------------------------------------
  # * Calculate Operated Value
  #     operation    : operation
  #     operand_type : operand type (0: invariable 1: variable)
  #     operand      : operand (number or variable ID)
  #--------------------------------------------------------------------------
  def operate_value(operation, operand_type, operand)
    # Get operand
    if operand_type == 0
      value = operand
    else
      value = $game_variables[operand]
    end
    # Reverse sign of integer if operation is [decrease]
    if operation == 1
      value = -value
    end
    # Return value
    value
  end
end
