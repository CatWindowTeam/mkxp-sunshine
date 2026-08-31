class Window_Settings
  class PresetsParameter < EnumParameter
    TYPE = :presets

    def initialize(settings_content, screen_id, position, name: "", additional_icons: [], icons: [], parameter: nil, init_value: 0, values: [], custom_text: "Custom", presets: [])
      @presets = presets

      super(settings_content, screen_id, position, name: name, additional_icons: additional_icons, icons: icons, parameter: parameter, init_value: init_value, values: [custom_text || "Custom"] + values)
    end
    
    def value=(value)
      super(value)
      if value > 0
        @presets[value - 1].each do |parameter, pvalue|
          Settings[parameter] = pvalue
          @settings_content.get_parameter_by_setting(parameter)&.redraw
        end
      end
    end

    def update
      super
      # TODO bind sigc signals to ruby and use signals
      custom = true
      @presets.each_with_index do |preset, index|
        use_this = true
        preset.each do |parameter, value|
          if Settings[parameter] != value
            use_this = false
            break
          end 
        end
        if use_this
          custom = false
          if @value != index + 1
            Settings[parameter] = index + 1
            redraw
          end
          break
        end
      end
      if custom && @value != 0
        Settings[parameter] = 0
        redraw
      end
    end

    def value_left()
      return if @disabled
      self.value = (self.value - 2) % [1, @max_value].max + 1
      redraw
    end
    def value_right()
      return if @disabled
      self.value = (self.value) % [1, @max_value].max + 1
      redraw
    end
  end
end