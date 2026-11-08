class Window_Settings
  class GamepadGUIDParameter < BaseParameter
    TYPE = :gamepad_guid

    def initialize(settings_content, screen_id, position, name: "", additional_icons: [], icons: [])
      super(settings_content, screen_id, position, name: name, additional_icons: additional_icons, icons: icons, parameter: nil, init_value: nil)
      @disabled = true
    end

    def value=(value)
      # do nothing
      nil
    end

    def get_display_value
      guid = Input::GamepadType.current_guid
      return guid || "00000000000000000000000000000000"
    end

    def redraw_value
      @sprite.bitmap.draw_text(Rect.new(-ICON_SIZE * 2 - 8, 0, PARAMETER_WIDTH, PARAMETER_HEIGHT), get_display_value, 2)
      x, y = GamepadIcons.icon()
      @sprite.bitmap.stretch_blt(
        Rect.new(PARAMETER_WIDTH - ICON_SIZE * 2, PARAMETER_HEIGHT / 2 - ICON_SIZE, ICON_SIZE * 2, ICON_SIZE * 2),
        RPG::Cache.menu("gamepad_icons"),
        Rect.new(x * ICON_SIZE, y * ICON_SIZE, ICON_SIZE, ICON_SIZE))
    end
  end
end