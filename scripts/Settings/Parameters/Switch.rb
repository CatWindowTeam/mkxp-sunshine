class Window_Settings
  class SwitchPatameter < BoolParameter
    TYPE = :switch

    def initialize(settings_content, screen_id, position, name, icon, parameter, bool_value, switch, invert)
      @switch = switch
      @invert = invert

      super(settings_content, screen_id, position, name, icon, parameter, bool_value)
    end
    
    def value=(value)
      super()

      if $game_switches[@switch]
        $game_switches[@switch] = @invert ? !@value : @value
      end
    end
  end
end