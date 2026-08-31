class Window_Settings
  class EnumParameter < IntParameter
    TYPE = :enum

    def initialize(settings_content, screen_id, position, name: "", additional_icons: [], icons: [], parameter: nil, init_value: 0, values: nil)
      @texts = values

      super(settings_content, screen_id, position, name: name, additional_icons: additional_icons, icons:icons, parameter: parameter, init_value: init_value, min: 0, max: values.length - 1)
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