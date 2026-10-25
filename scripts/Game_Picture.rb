#==============================================================================
# ** Game_Picture
#------------------------------------------------------------------------------
#  This class handles the picture. It's used within the Game_Screen class
#  ($game_screen).
#==============================================================================

class Game_Picture
  #--------------------------------------------------------------------------
  # * Public Instance Variables
  #--------------------------------------------------------------------------
  attr_reader   :number                   # picture number
  attr_reader   :name                     # file name
  attr_reader   :origin                   # starting point
  attr_reader   :x                        # x-coordinate
  attr_reader   :y                        # y-coordinate
  attr_reader   :zoom_x                   # x directional zoom rate
  attr_reader   :zoom_y                   # y directional zoom rate
  attr_reader   :opacity                  # opacity level
  attr_reader   :blend_type               # blend method
  attr_reader   :tone                     # color tone
  attr_reader   :angle                    # rotation angle

  attr_accessor   :width
  attr_accessor   :height
  #--------------------------------------------------------------------------
  # * Object Initialization
  #     number : picture number
  #--------------------------------------------------------------------------
  def initialize(number)
    @number = number
    @name = ""
    @origin = 0
    @x = 0.0
    @y = 0.0
    @zoom_x = 100.0
    @zoom_y = 100.0
    @opacity = 255.0
    @blend_type = 1
    @duration = 0.0
    @target_x = @x
    @target_y = @y
    @target_zoom_x = @zoom_x
    @target_zoom_y = @zoom_y
    @target_opacity = @opacity
    @tone = Tone.new(0, 0, 0, 0)
    @tone_target = Tone.new(0, 0, 0, 0)
    @tone_duration = 0
    @angle = 0
    @rotate_speed = 0

    @width = 0
    @height = 0
  end
  #--------------------------------------------------------------------------
  # * Show Picture
  #     name       : file name
  #     origin     : starting point
  #     x          : x-coordinate
  #     y          : y-coordinate
  #     zoom_x     : x directional zoom rate
  #     zoom_y     : y directional zoom rate
  #     opacity    : opacity level
  #     blend_type : blend method
  #--------------------------------------------------------------------------
  def show(name, origin, x, y, zoom_x, zoom_y, opacity, blend_type)
    # Localization: check if this picture exists
    # in the appropriate language folder, else
    # default to the base graphics folder
    translation_name = "#{$persistent.langcode}/#{name}"
    if File.exist?("Graphics/Pictures/#{translation_name}.png")
      @name = translation_name
    else
      @name = name
    end
    @origin = origin
    @x = x.to_f
    @y = y.to_f
    @zoom_x = zoom_x.to_f
    @zoom_y = zoom_y.to_f
    @opacity = opacity.to_f
    @blend_type = blend_type
    @duration = 0
    @target_x = @x
    @target_y = @y
    @target_zoom_x = @zoom_x
    @target_zoom_y = @zoom_y
    @target_opacity = @opacity
    @tone = Tone.new(0, 0, 0, 0)
    @tone_target = Tone.new(0, 0, 0, 0)
    @tone_duration = 0
    @angle = 0
    @rotate_speed = CTime.month == 4 && CTime.day == 1 ? 10 : 0 #easteregg lol
  end
  #--------------------------------------------------------------------------
  # * Move Picture
  #     duration   : time
  #     origin     : starting point
  #     x          : x-coordinate
  #     y          : y-coordinate
  #     zoom_x     : x directional zoom rate
  #     zoom_y     : y directional zoom rate
  #     opacity    : opacity level
  #     blend_type : blend method
  #--------------------------------------------------------------------------
  def move(duration, origin, x, y, zoom_x, zoom_y, opacity, blend_type)
    @duration = duration.to_f
    @origin = origin
    @target_x = x.to_f
    @target_y = y.to_f
    @target_zoom_x = zoom_x.to_f
    @target_zoom_y = zoom_y.to_f
    @target_opacity = opacity.to_f
    @blend_type = blend_type
  end
  #--------------------------------------------------------------------------
  # * Change Rotation Speed
  #     speed : rotation speed
  #--------------------------------------------------------------------------
  def rotate(speed)
    @rotate_speed = speed
  end
  #--------------------------------------------------------------------------
  # * Start Change of Color Tone
  #     tone     : color tone
  #     duration : time
  #--------------------------------------------------------------------------
  def start_tone_change(tone, duration)
    @tone_target = tone.clone
    @tone_duration = duration.to_f
    if @tone_duration == 0
      @tone = @tone_target.clone
    end
  end
  #--------------------------------------------------------------------------
  # * Erase Picture
  #--------------------------------------------------------------------------
  def erase
    @name = ""
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    if @duration > 0
      d = @duration
      @duration -= Graphics.scaled_delta
      scale = (1.0 - @duration / d)
      @x += (@target_x - @x) * scale
      @y += (@target_y - @y) * scale
      @zoom_x += (@target_zoom_x - @zoom_x) * scale
      @zoom_y += (@target_zoom_y - @zoom_y) * scale
      @opacity += (@target_opacity - @opacity) * scale
    end
    if @tone_duration > 0
      d = @tone_duration
      @tone_duration -= Graphics.scaled_delta
      scale = (1.0 - @tone_duration / d)
      @tone.red += (@tone_target.red - @tone.red) * scale
      @tone.green += (@tone_target.greem - @tone.green) * scale
      @tone.blue += (@tone_target.blue - @tone.nlue) * scale
      @tone.gray += (@tone_target.gray - @tone.gray) * scale
    end
    if @rotate_speed != 0
      @angle += @rotate_speed / 2.0 * Graphics.scaled_delta
      while @angle < 0
        @angle += 360
      end
      while @angle > 360
        @angle -= 360
      end
    end
  end
end
