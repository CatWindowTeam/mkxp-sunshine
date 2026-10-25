#==============================================================================
# ** Game_Screen
#------------------------------------------------------------------------------
#  This class handles screen maintenance data, such as change in color tone,
#  flashing, etc. Refer to "$game_screen" for the instance of this class.
#==============================================================================

class Game_Screen
  #--------------------------------------------------------------------------
  # * Public Instance Variables
  #--------------------------------------------------------------------------
  attr_reader   :tone                     # color tone
  attr_reader   :flash_color              # flash color
  attr_reader   :shake                    # shake positioning
  attr_reader   :pictures                 # pictures
  attr_reader   :weather_type             # weather type
  attr_reader   :weather_max              # max number of weather sprites
  #--------------------------------------------------------------------------
  # * Object Initialization
  #--------------------------------------------------------------------------
  def initialize
    @tone = Tone.new(0, 0, 0, 0)
    @tone_target = Tone.new(0, 0, 0, 0)
    @tone_duration = 0
    @flash_color = Color.new(0, 0, 0, 0)
    @flash_duration = 0
    @shake_power = 0
    @shake_speed = 0
    @shake_duration = 0
    @shake_direction = 1
    @shake = 0
    @pictures = [nil]
    (1..100).each do |i|
      @pictures.push(Game_Picture.new(i))
    end
    @weather_type = 0
    @weather_max = 0.0
    @weather_type_target = 0
    @weather_max_target = 0.0
    @weather_duration = 0
  end
  #--------------------------------------------------------------------------
  # * Start Changing Color Tone
  #     tone : color tone
  #     duration : time
  #--------------------------------------------------------------------------
  def start_tone_change(tone, duration)
    @tone_target = tone.clone
    @tone_duration = duration
    if @tone_duration == 0
      @tone = @tone_target.clone
    end
  end
  #--------------------------------------------------------------------------
  # * Start Flashing
  #     color : color
  #     duration : time
  #--------------------------------------------------------------------------
  def start_flash(color, duration)
    @flash_color = color.clone
    @flash_duration = duration
  end
  #--------------------------------------------------------------------------
  # * Start Shaking
  #     power : strength
  #     speed : speed
  #     duration : time
  #--------------------------------------------------------------------------
  def start_shake(power, speed, duration)
    @shake_power = power
    @shake_speed = speed
    @shake_duration = duration
  end
  #--------------------------------------------------------------------------
  # * Set Weather
  #     type : type
  #     power : strength
  #     duration : time
  #--------------------------------------------------------------------------
  def weather(type, power, duration)
    @weather_type_target = type
    if @weather_type_target != 0
      @weather_type = @weather_type_target
    end
    if @weather_type_target == 0
      @weather_max_target = 0.0
    else
      @weather_max_target = (power + 1) * 4.0
    end
    @weather_duration = duration
    if @weather_duration == 0
      @weather_type = @weather_type_target
      @weather_max = @weather_max_target
    end
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    if @tone_duration > 0
      d = @tone_duration
      @tone_duration -= Graphics.scaled_delta
      scale = (1.0 - @tone_duration / d)
      @tone.red += (@tone_target.red - @tone.red) * scale
      @tone.green += (@tone_target.green - @tone.green) * scale
      @tone.blue += (@tone_target.blue - @tone.blue) * scale
      @tone.gray += (@tone_target.gray - @tone.gray) * scale
    else
      @tone.red = @tone_target.red
      @tone.green = @tone_target.green
      @tone.blue = @tone_target.blue
      @tone.gray = @tone_target.gray
      @tone_duration = 0
    end
    if @flash_duration > 0
      d = @flash_duration
      @flash_duration -= Graphics.scaled_delta
      @flash_color.alpha -= @flash_color.alpha * (1.0 - @flash_duration / d)
    else
      @flash_color.alpha = 0
      @flash_duration = 0
    end
    if @shake_duration >= 1 || @shake != 0
      delta = (@shake_power * @shake_speed * @shake_direction) / 10.0
      if @shake_duration <= 1 && @shake * (@shake + delta) < 0
        @shake = 0
      else
        @shake += delta
      end
      if @shake > @shake_power * 2
        @shake_direction = -1
      end
      if @shake < - @shake_power * 2
        @shake_direction = 1
      end
      if @shake_duration > 0
        @shake_duration -= Graphics.scaled_delta
      end
    end
    if @weather_duration > 0
      d = @weather_duration
      @weather_max = (@weather_max * (d - 1) + @weather_max_target) / d
      @weather_duration -= Graphics.scaled_delta
      if @weather_duration == 0
        @weather_type = @weather_type_target
      end
    end
    if $game_temp.in_battle
      (51..100).each do |i|
        @pictures[i].update
      end
    else
      (1..50).each do |i|
        @pictures[i].update
      end
    end
  end
end
