# The main menu window
class Window_TPtL < Window_Selectable
  def initialize
    super(Graphics.width, 64, Graphics.height, 64)
    @mapinfo = load_data("Data/MapInfos.rxdata")
		
    @item_max = 262
    @column_max = @item_max

    # Make invisible by default
    self.visible = false
    self.active = false
    self.back_opacity = 230
    @fade_in = false
    @fade_out = false

    # Render menu
    self.contents = Bitmap.new(width - 32, 32)
    Language.register_text_sprite(self.class.name + "_contents", self.contents)
    for i in 0...@item_max
      draw_item(i, normal_color)
    end
    self.z = 9998
  end
  #--------------------------------------------------------------------------
  # * Dispose
  #--------------------------------------------------------------------------
  def dispose
    # Dispose of windows
    super
  end
  #--------------------------------------------------------------------------
  # * Draw Item
  #     index : item number
  #     color : text color
  #--------------------------------------------------------------------------
  def draw_item(index, color)
    # Set color
    self.contents.font.color = color

    # Get width of text and cap
    w = self.width / @item_max

    # Update item
    rect = Rect.new(w * index, 0, w - 32, 32)
    self.contents.fill_rect(rect, Color.new(0, 0, 0, 0))
    self.contents.draw_text(rect, tr(@mapinfo&.[](index)&.name || "EMPTY"), 1)
  end
  #--------------------------------------------------------------------------
  # * Disable Item
  #     index : item number
  #--------------------------------------------------------------------------
  def disable_item(index)
    draw_item(index, disabled_color)
  end

  # Open/show the menu
  def open
    # redraw in case language has been updated
    for i in 0...@item_max
      draw_item(i, normal_color)
    end
    self.opacity = 0
    self.contents_opacity = 0
    self.visible = true
    @fade_in = true
    self.index = 0
    self.active = true
  end

  # Update
  def update
    super

    # Handle fade-in effect
    if @fade_in
      if Input.trigger?(Input::ITEMS)
        @fade_in = false
        @fade_out = true
      else
        self.opacity += 48
        self.contents_opacity += 48
        if self.contents_opacity == 255
          @fade_in = false
        end
        return
      end
    end

    # Handle fade-out effect
    if @fade_out
      self.opacity -= 48
      self.contents_opacity -= 48
      if self.contents_opacity == 0
        @fade_out = false
        self.visible = false
        self.active = false
      end
      return
    end

    # Don't do anything if not active
    if !self.active || $game_system.map_interpreter.running?
      return
    end


    # Cancel menu
    if Input.trigger?(Input::CANCEL) ||
        Input.trigger?(Input::MENU) ||
        Input.trigger?(Input::ITEMS)
      unless Input.trigger?(Input::ITEMS)
        $game_system.se_play($data_system.cancel_se)
      end
      @fade_out = true
      return
    end
  end
	
    # Select menu item
    if Input.trigger?(Input::ACTION)
      self.active = false
      self.opacity = 127
	  $game_temp.player_transferring = true
	  $game_temp.player_new_map_id = @index
	  $game_temp.player_new_x = 0
	  $game_temp.player_new_y = 0
	  $game_temp.player_new_direction = 0
	  Graphics.freeze
	  $game_temp.transition_processing = true
	  $game_temp.transition_name = ""
	  @fade_out = true
  end
end
