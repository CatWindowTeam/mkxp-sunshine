# Modifications for input-binding.cpp
module Input
  class << self
    alias_method(:c_axis_pressure_int, :c_axis_pressure)

    def c_axis_pressure(axis)
      normalized = c_axis_pressure_int(axis).to_f / 32767.0 
      return normalized < Settings[:gamepad_deadzone] / 10.0 ? 0.0 : normalized
    end
  end
end
