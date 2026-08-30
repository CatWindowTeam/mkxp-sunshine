class Window_Settings
  class FloatParameter < BaseParameter
    TYPE = :float
    
    attr_reader :max_value
    attr_reader :min_value
    attr_reader :step

    def initialize(settings_content, screen_id, position, name, icon, parameter, float_value, step, min_value, max_value)
      @step = step
      @min_value = min_value
      @max_value = max_value

      super(settings_content, screen_id, position, name, icon, parameter, float_value)
    end

    def value=(value)
      if (value != @value)
        Audio.se_play(PARAMETER_CHANGE_AUDIO, 70, ((value + @min_value) / (@max_value + @min_value) * 50.0).to_i + 75)
      end
      super(value)
    end

    def value_left()
      return if @disabled
      self.value = (self.value - @step).clamp(@min_value, @max_value)
      redraw
    end
    def value_right()
      return if @disabled
      self.value = (self.value + @step).clamp(@min_value, @max_value)
      redraw
    end
  end
end