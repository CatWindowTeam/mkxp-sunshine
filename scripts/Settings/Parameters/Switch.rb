class Window_Settings
  class SwitchPatameter < BoolParameter
    TYPE = :switch

    def initialize(settings_content, screen_id, position, name: "", additional_icons: [], icons: [], parameter: nil, init_value: false, switch: nil, invert: false)
      @switch = switch
      @invert = invert

      super(settings_content, screen_id, position, name: name, additional_icons: additional_icons, icons: icons, parameter: parameter, init_value: init_value)
    end
    
    def value=(value)
      super()

      if $game_switches[@switch]
        $game_switches[@switch] = @invert ? !@value : @value
      end
    end
  end
end