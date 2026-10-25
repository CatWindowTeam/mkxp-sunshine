#PRIORITY 51

#==============================================================================
# ** Window_Base
#------------------------------------------------------------------------------
#  This class is for all in-game windows.
#==============================================================================

class Window_Base < Window
  #--------------------------------------------------------------------------
  # * Object Initialization
  #     x      : window x-coordinate
  #     y      : window y-coordinate
  #     width  : window width
  #     height : window height
  #--------------------------------------------------------------------------
  def initialize(x, y, width, height)
    super()
    @windowskin_name = $game_system.windowskin_name
   	self.windowskin = RPG::Cache.windowskin(@windowskin_name)
    self.x = x
    self.y = y
    self.width = width
    self.height = height
    self.order = 100
  end
  #--------------------------------------------------------------------------
  # * Dispose
  #--------------------------------------------------------------------------
  def dispose
    # Dispose if window contents bit map is set
    if self.contents != nil
      self.contents.dispose
    end
    super
  end
  #--------------------------------------------------------------------------
  # * Get Text Color
  #     n : text color number (0-7)
  #--------------------------------------------------------------------------
  def self.text_color(n)
    case n
    when 0
      Color.new(255, 255, 255, 255)
    when 1
      Color.new(255, 64, 64, 255)
    when 2
      Color.new(0, 224, 0, 255)
    when 3
      Color.new(255, 255, 0, 255)
    when 4
      Color.new(64, 64, 255, 255)
    when 5
      Color.new(255, 64, 255, 255)
    when 6
      Color.new(64, 255, 255, 255)
    when 7
      Color.new(128, 128, 128, 255)
    else
      normal_color
    end
  end
  #--------------------------------------------------------------------------
  # * Get Normal Text Color
  #--------------------------------------------------------------------------
  def normal_color
    Color.new(255, 255, 255, 255)
  end
  #--------------------------------------------------------------------------
  # * Get Active Item Text Color
  #--------------------------------------------------------------------------
  def active_item_color
    Color.new(222, 134, 0, 255)
  end
  #--------------------------------------------------------------------------
  # * Get Disabled Text Color
  #--------------------------------------------------------------------------
  def disabled_color
    Color.new(255, 255, 255, 128)
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    super
    # Reset if windowskin was changed
    if $game_system.windowskin_name != @windowskin_name
      @windowskin_name = $game_system.windowskin_name
      self.windowskin = RPG::Cache.windowskin(@windowskin_name)
    end
  end
end

