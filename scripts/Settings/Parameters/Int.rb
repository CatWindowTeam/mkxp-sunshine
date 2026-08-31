#PRIORITY 9

class Window_Settings
  class IntParameter < BaseParameter
    TYPE = :int
    
    attr_reader :max_value
    attr_reader :min_value

    def initialize(settings_content, screen_id, position, name: "", additional_icons: [], icons: [], parameter: nil, init_value: 0, min: 0, max: 10)
      @min_value = min
      @max_value = max

      super(settings_content, screen_id, position, name: name, additional_icons: additional_icons, icons: icons, parameter: parameter, init_value: init_value)
    end

    def value=(value)
      if (value != self.value)
        Audio.se_play(PARAMETER_CHANGE_AUDIO, 70, ((value.to_f + @min_value.to_f) / (@max_value.to_f + @min_value.to_f) * 50.0).to_i + 75)
      end
      super(value)
    end

    def value_left()
      return if @disabled
      self.value = (self.value - 1).clamp(@min_value, @max_value)
      redraw
    end
    def value_right()
      return if @disabled
      self.value = (self.value + 1).clamp(@min_value, @max_value)
      redraw
    end
  end
end