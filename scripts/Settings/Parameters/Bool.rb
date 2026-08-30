class Window_Settings
  class BoolParameter < BaseParameter
    TYPE = :bool

    def initialize(settings_content, screen_id, position, name, icon, parameter, bool_value)
      super(settings_content, screen_id, position, name, icon, parameter, bool_value || false)
      @value_width = BOOL_VALUE_WIDTH
    end

    def value=(value)
      if (value != self.value)
        Audio.se_play(PARAMETER_CHANGE_AUDIO, 70, value ? 125 : 75)
      end
      super(value)
    end

    def get_display_value
      return self.value ? "ON" : "OFF"
    end
    
    def value_left()
      return if @disabled
      self.value = !self.value
      redraw
    end
    def value_right()
      return if @disabled
      self.value = !self.value
      redraw
    end
    def action()
      return if @disabled
      self.value = !self.value
      redraw
    end
  end
end