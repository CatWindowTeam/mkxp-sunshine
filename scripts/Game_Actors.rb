#==============================================================================
# ** Game_Actors
#------------------------------------------------------------------------------
#  This class handles the actor array. Refer to "$game_actors" for each
#  instance of this class.
#==============================================================================

class Game_Actors
  #--------------------------------------------------------------------------
  # * Object Initialization
  #--------------------------------------------------------------------------
  def initialize
    @data = []
    RPG::Mod.exec_hooks("hooks/Game_Actors/init", binding)
  end
  #--------------------------------------------------------------------------
  # * Get Actor
  #     actor_id : actor ID
  #--------------------------------------------------------------------------
  def [](actor_id)
    if actor_id > 999 or $data_actors[actor_id] == nil
      return nil
    end
    if @data[actor_id] == nil
      @data[actor_id] = Game_Actor.new(actor_id)
    end
    return @data[actor_id]
  end
end
