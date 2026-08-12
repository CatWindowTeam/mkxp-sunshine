#==============================================================================
# ** Scene_Title
#------------------------------------------------------------------------------
#  This class performs title screen processing.
#==============================================================================

class Scene_Title
  
  #--------------------------------------------------------------------------
  # * Main Processing
  #--------------------------------------------------------------------------
  def main

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
      # Abort loop if screen is changed
      if $scene != self
        break
      end
    end
  end

  def update
  
  end
end
