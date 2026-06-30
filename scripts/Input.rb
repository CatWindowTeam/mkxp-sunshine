# Modifications for input-binding.cpp
module Input
  class << self
    alias_method(:c_axis_pressure_int, :c_axis_pressure)

    def c_axis_pressure(axis)
      normalized = c_axis_pressure_int(axis).to_f / 32767.0 
      return normalized < Settings[:gamepad_deadzone] / 10.0 ? 0.0 : normalized
    end
  end

  GAMEPAD_BUTTONS_NAMES = {
    "a" => "A", # ▲□Ο
    "b" => "B",
    "x" => "X",
    "y" => "Y",
    "lefttringger" => "LT",
    "righttringger" => "RT",
    "leftshoulder" => "LS",
    "rightshoulder" => "RS",
    "dpdown" => "D-Pad Down",
    "dpleft" => "D-Pad Left",
    "dpright" => "D-Pad Right",
    "dpup" => "D-Pad Up",
    "back" => "Back",
    "guide" => "Guide",
    "start" => "Start",
    "leftstick" => "Left Stick",
    "rightstick" => "Right Stick",
    "leftpaddle1" => "Left Paddle 1",
    "rightpaddle1" => "Right Paddle 1",
    "leftpaddle2" => "Left Paddle 2",
    "rightpaddle2" => "Right Paddle 2",
    "touchpad" => "Touchpad",
    "misc1" => "Misc 1",
    "misc2" => "Misc 2",
    "misc3" => "Misc 3",
    "misc4" => "Misc 4",
    "misc5" => "Misc 5",
    "misc6" => "Misc 6",
  }
end