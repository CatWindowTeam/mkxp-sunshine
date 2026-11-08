class Window_Settings
  class GamepadAxisIconParameter < BaseParameter
    TYPE = :gamepad_axis

    def initialize(settings_content, screen_id, position, name: "", additional_icons: [], icons: [], axis: nil)
      @icon = axis
      super(settings_content, screen_id, position, name: name, additional_icons: additional_icons, icons: icons, parameter: nil, init_value: nil)
      @disabled = true
    end

    def value=(value)
      # do nothing
      nil
    end

    def get_display_value
      return ""
    end

    def redraw_value
      x, y = GamepadIcons.axis(@icon, 1)
      @sprite.bitmap.stretch_blt(
        Rect.new(PARAMETER_WIDTH - ICON_SIZE * 2, PARAMETER_HEIGHT / 2 - ICON_SIZE, ICON_SIZE * 2, ICON_SIZE * 2),
        RPG::Cache.menu("gamepad_icons"),
        Rect.new(x * ICON_SIZE, y * ICON_SIZE, ICON_SIZE, ICON_SIZE))

      x, y = GamepadIcons.axis(@icon, 0)
      @sprite.bitmap.stretch_blt(
        Rect.new(PARAMETER_WIDTH - ICON_SIZE * 4 - 8, PARAMETER_HEIGHT / 2 - ICON_SIZE, ICON_SIZE * 2, ICON_SIZE * 2),
        RPG::Cache.menu("gamepad_icons"),
        Rect.new(x * ICON_SIZE, y * ICON_SIZE, ICON_SIZE, ICON_SIZE))
    end
  end
end