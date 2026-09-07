class Window_Settings
  class FloatParameter < BaseParameter
    TYPE = :float
    
    attr_reader :max_value
    attr_reader :min_value
    attr_reader :step

    def initialize(settings_content, screen_id, position, name: "", additional_icons: [], icons: [], parameter: nil, init_value: 0.0, step: 0.1, min: 0.0, max: 1.0)
      @step = step
      @min_value = min
      @max_value = max

      super(settings_content, screen_id, position, name: name, additional_icons: additional_icons, icons: icons, parameter: parameter, init_value: init_value)
    end

    def value=(value)
      if (value != @value)
        @settings_content.play_param(((value + @min_value) / (@max_value + @min_value) * 0.5).to_i + 0.75)
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