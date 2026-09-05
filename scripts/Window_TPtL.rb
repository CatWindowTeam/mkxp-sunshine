class Window_TPtL < Window_Selectable
  def initialize
    super(16, 16, Graphics.width - 32, Graphics.height - 32)
    @mapinfos = load_data("Data/MapInfos.rxdata")
    @tree = []
    sort

    @item_max = @mapinfos.length;

    # Make invisible by default
    self.visible = false
    self.active = false
    self.back_opacity = 230
    @fade_in = false
    @fade_out = false

    # Render menu
    self.contents = Bitmap.new(width - 32, @item_max * 32)
    Language.register_text_sprite(self.class.name + "_contents", self.contents)

    ix = 0
    @mapinfos.each do |map_id, map_info|
      draw_item(map_id, ix, normal_color)
      ix += 1
    end
    
    self.z = 9998
  end

  def sort
    @mapinfos.each do |map_id, map_info|
      if map_info.parent_id == 0
        @tree << [map_id, []]
      elsif @mapinfos.has_key?(map_info.parent_id)
        add_item(map_id)
      end
    end

    result = {}
    add_items(@tree, result)
    @mapinfos = result
  end

  def add_item(map_id)
    if get_item(@tree, map_id) != nil
      return
    end
    if (@mapinfos[map_id].parent_id == 0)
      @tree << [map_id, []]
      return
    end

    info = @mapinfos[map_id]

    parent = @tree
    item = get_item(@tree, info.parent_id)
    if item != nil
      parent = item
    else
      add_item(info.parent_id)
      item = get_item(@tree, info.parent_id)
      if item != nil
        parent = item
      end
    end

    parent << [map_id, []]
  end

  def get_item(parent, map_id)
    parent.each do |item|
      if (item[0] == map_id)
        return item[1]
      else
        i = get_item(item[1], map_id)
        if i != nil
          return i
        end
      end
    end
    return nil
  end

  def add_items(tree, hash)
    tree.each do |item|
      hash[item[0]] = @mapinfos[item[0]]
      unless item[1].length == 0
        add_items(item[1], hash)
      end
    end
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
  def draw_item(index, draw_ix, color)
    map_info = @mapinfos[index]

    indent = 0;
    last_map = index
    while (last_map != 0)
      indent += 1
      last_map = @mapinfos[last_map].parent_id
    end

    # Set color
    self.contents.font.color = color

    # Update item
    self.contents.draw_text(Rect.new(indent * 64, 32 * draw_ix, self.width - indent * 64, 32), map_info&.name || "UNKNOWN_MAP", 0)
    self.contents.draw_text(Rect.new(0, 32 * draw_ix, self.width - 38, 32), index.to_s, 2)
  end

  # Open/show the menu
  def open
    # redraw in case language has been updated
    
    ix = 0
    @mapinfos.each do |map_id, map_info|
      draw_item(map_id, ix, normal_color)
      ix += 1
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
	
    # Select menu item
    if Input.trigger?(Input::ACTION)
      self.active = false
      self.opacity = 127
      $game_temp.player_transferring = true
      $game_temp.player_new_map_id = @mapinfos.keys[@index]
      $game_temp.player_new_x = 0
      $game_temp.player_new_y = 0
      $game_temp.player_new_direction = 0
      $game_temp.transition_processing = true
      $game_temp.transition_name = ""
      @fade_out = true
    end
  end
end
