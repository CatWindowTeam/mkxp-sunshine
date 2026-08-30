class Window_Settings
  class EnumParameter < IntParameter
    TYPE = :enum

    def initialize(settings_content, screen_id, position, name, icon, parameter, int_value, enum_texts)
      @texts = enum_texts

      super(settings_content, screen_id, position, name, icon, parameter, int_value, 0, enum_texts.length - 1)
    end

    def get_display_value
      @texts&.[](self.value) || "#{tr("Unknown value")} #{self.value}"
    end

    def value_left()
      return if @disabled
      self.value = (self.value - 1) % [1, @max_value + 1].max
      redraw
    end
    def value_right()
      return if @disabled
      self.value = (self.value + 1) % [1, @max_value + 1].max
      redraw
    end
  end
end