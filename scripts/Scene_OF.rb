class Scene_OF
  BPM = 100
  BPM_TIME = 1.0 / (BPM / 60.0) * Graphics.frame_rate * 2

  BUMP_SCALE = 1.07
  BUMP_RETURN_SPEED = 0.1
  ICONS_BUMP_SCALE = 1.17
  ICONS_BUMP_RETURN_SPEED = 0.2

  ARROW_SIZE = 32
  ARROW_SCALE = 2
  ARROWS_PADDING = 12
  ARROWS_TOP_MARGIN = 32
  ARROWS_SIDES_MARGIN = 48

  HALF_OF_ARROW = ARROW_SIZE * ARROW_SCALE / 2

  ARROWS_SPEED = 10
  ARROWS_ALLOW_ZONE_SIZE = 50

  HP_WIDTH = 400
  HP_HEIGHT = 16

  MISS_TIME = 60

  SPRITES = {
    :cat_base_arrow_left       => Rect.new(0,   0, 32, 32),   :cat_base_arrow_down       => Rect.new(32,   0, 32, 32),  :cat_base_arrow_up       => Rect.new(64,   0, 32, 32),  :cat_base_arrow_right       => Rect.new(96,   0, 32, 32),
    :cat_pressed_arrow_left    => Rect.new(0,  32, 32, 32),   :cat_pressed_arrow_down    => Rect.new(32,  32, 32, 32),  :cat_pressed_arrow_up    => Rect.new(64,  32, 32, 32),  :cat_pressed_arrow_right    => Rect.new(96,  32, 32, 32),
    :cat_target_arrow_left     => Rect.new(0,  64, 32, 32),   :cat_target_arrow_down     => Rect.new(32,  64, 32, 32),  :cat_target_arrow_up     => Rect.new(64,  64, 32, 32),  :cat_target_arrow_right     => Rect.new(96,  64, 32, 32),
    :cat_best_arrow_left       => Rect.new(0,  96, 32, 32),   :cat_best_arrow_down       => Rect.new(32,  96, 32, 32),  :cat_best_arrow_up       => Rect.new(64,  96, 32, 32),  :cat_best_arrow_right       => Rect.new(96,  96, 32, 32),
    :cat_tail_left             => Rect.new(0, 128, 32, 16),   :cat_tail_down             => Rect.new(32, 128, 32, 16),  :cat_tail_up             => Rect.new(64, 128, 32, 16),  :cat_tail_right             => Rect.new(96, 128, 32, 16), 
    :cat_tail_end_left         => Rect.new(0, 144, 32, 16),   :cat_tail_end_down         => Rect.new(32, 144, 32, 16),  :cat_tail_end_up         => Rect.new(64, 144, 32, 16),  :cat_tail_end_right         => Rect.new(96, 144, 32, 16), 
    
    :player_base_arrow_left    => Rect.new(128,   0, 32, 32), :player_base_arrow_down    => Rect.new(160,   0, 32, 32), :player_base_arrow_up    => Rect.new(192,   0, 32, 32), :player_base_arrow_right    => Rect.new(224,   0, 32, 32),
    :player_pressed_arrow_left => Rect.new(128,  32, 32, 32), :player_pressed_arrow_down => Rect.new(160,  32, 32, 32), :player_pressed_arrow_up => Rect.new(192,  32, 32, 32), :player_pressed_arrow_right => Rect.new(224,  32, 32, 32),
    :player_target_arrow_left  => Rect.new(128,  64, 32, 32), :player_target_arrow_down  => Rect.new(160,  64, 32, 32), :player_target_arrow_up  => Rect.new(192,  64, 32, 32), :player_target_arrow_right  => Rect.new(224,  64, 32, 32),
    :player_best_arrow_left    => Rect.new(128,  96, 32, 32), :player_best_arrow_down    => Rect.new(160,  96, 32, 32), :player_best_arrow_up    => Rect.new(192,  96, 32, 32), :player_best_arrow_right    => Rect.new(224,  96, 32, 32),
    :player_tail_left          => Rect.new(128, 128, 32, 16), :player_tail_down          => Rect.new(160, 128, 32, 16), :player_tail_up          => Rect.new(192, 128, 32, 16), :player_tail_right          => Rect.new(224, 128, 32, 16), 
    :player_tail_end_left      => Rect.new(128, 144, 32, 16), :player_tail_end_down      => Rect.new(160, 144, 32, 16), :player_tail_end_up      => Rect.new(192, 144, 32, 16), :player_tail_end_right      => Rect.new(224, 144, 32, 16), 
  

    :player_idle1 => Rect.new(0, 160, 128, 128), :player_idle2 => Rect.new(128, 160, 128, 128), :player_idle3 => Rect.new(256, 160, 128, 128), :player_idle4 => Rect.new(384, 160, 128, 128),
    :player_sign_left1  => Rect.new(0, 288, 128, 128), :player_sign_left2  => Rect.new(128, 288, 128, 128), :player_sign_left3  => Rect.new(256, 288, 128, 128), :player_miss_left  => Rect.new(384, 288, 128, 128),
    :player_sign_down1  => Rect.new(0, 416, 128, 128), :player_sign_down2  => Rect.new(128, 416, 128, 128), :player_sign_down3  => Rect.new(256, 416, 128, 128), :player_miss_down  => Rect.new(384, 416, 128, 128),
    :player_sign_up1    => Rect.new(0, 554, 128, 128), :player_sign_up2    => Rect.new(128, 554, 128, 128), :player_sign_up3    => Rect.new(256, 554, 128, 128), :player_miss_up    => Rect.new(384, 554, 128, 128),
    :player_sign_right1 => Rect.new(0, 682, 128, 128), :player_sign_right2 => Rect.new(128, 682, 128, 128), :player_sign_right3 => Rect.new(256, 682, 128, 128), :player_miss_right => Rect.new(384, 682, 128, 128),

    :cat_idle1 => Rect.new(0, 800, 128, 128), :cat_idle2 => Rect.new(128, 800, 128, 128), :cat_idle3 => Rect.new(256, 800, 128, 128), :cat_idle4 => Rect.new(384, 800, 128, 128),
    :cat_sign_left1  => Rect.new(0,  928, 128, 128), :cat_sign_left2  => Rect.new(128,  928, 128, 128), :cat_sign_left3  => Rect.new(256,  928, 128, 128), :cat_miss_left  => Rect.new(384,  928, 128, 128),
    :cat_sign_down1  => Rect.new(0, 1056, 128, 128), :cat_sign_down2  => Rect.new(128, 1056, 128, 128), :cat_sign_down3  => Rect.new(256, 1056, 128, 128), :cat_miss_down  => Rect.new(384, 1056, 128, 128),
    :cat_sign_up1    => Rect.new(0, 1194, 128, 128), :cat_sign_up2    => Rect.new(128, 1194, 128, 128), :cat_sign_up3    => Rect.new(256, 1194, 128, 128), :cat_miss_up    => Rect.new(384, 1194, 128, 128),
    :cat_sign_right1 => Rect.new(0, 1322, 128, 128), :cat_sign_right2 => Rect.new(128, 1322, 128, 128), :cat_sign_right3 => Rect.new(256, 1322, 128, 128), :cat_miss_right => Rect.new(384, 1322, 128, 128),
  
    :hpbar_cat_lose    => Rect.new(320,  0, 32, 32), :hpbar_cat_normal    => Rect.new(288,  0, 32, 32), :hpbar_cat_win    => Rect.new(256,  0, 32, 32),
    :hpbar_player_lose => Rect.new(320, 32, 32, 32), :hpbar_player_normal => Rect.new(288, 32, 32, 32), :hpbar_player_win => Rect.new(256, 32, 32, 32),
  }

  PLAYER_ARROWS_SPRITES = {
    :base     => [:player_base_arrow_left,    :player_base_arrow_down,    :player_base_arrow_up,    :player_base_arrow_right    ],
    :pressed  => [:player_pressed_arrow_left, :player_pressed_arrow_down, :player_pressed_arrow_up, :player_pressed_arrow_right ],
    :target   => [:player_target_arrow_left,  :player_target_arrow_down,  :player_target_arrow_up,  :player_target_arrow_right  ],
    :best     => [:player_best_arrow_left,    :player_best_arrow_down,    :player_best_arrow_up,    :player_best_arrow_right    ],
    :tail     => [:player_tail_left,          :player_tail_down,          :player_tail_up,          :player_tail_right          ],
    :tail_end => [:player_tail_end_left,      :player_tail_end_down,      :player_tail_end_up,      :player_tail_end_right      ],
  }

  CAT_ARROWS_SPRITES = {
    :base     => [:cat_base_arrow_left,    :cat_base_arrow_down,    :cat_base_arrow_up,    :cat_base_arrow_right    ],
    :pressed  => [:cat_pressed_arrow_left, :cat_pressed_arrow_down, :cat_pressed_arrow_up, :cat_pressed_arrow_right ],
    :target   => [:cat_target_arrow_left,  :cat_target_arrow_down,  :cat_target_arrow_up,  :cat_target_arrow_right  ],
    :best     => [:cat_best_arrow_left,    :cat_best_arrow_down,    :cat_best_arrow_up,    :cat_best_arrow_right    ],
    :tail     => [:cat_tail_left,          :cat_tail_down,          :cat_tail_up,          :cat_tail_right          ],
    :tail_end => [:cat_tail_end_left,      :cat_tail_end_down,      :cat_tail_end_up,      :cat_tail_end_right      ],
  }
  
  def main
    Audio.bgm_stop

    @arrows_states = [false, false, false, false]

    @instruments = Audio.create_sound("Audio/.cats", false, Audio.music_group)
    length = @instruments.length
    @instruments.max_frame = length / 3
    @voice_cat = Audio.create_sound("Audio/.cats", false, Audio.music_group)
    @voice_cat.start_frame = length / 6
    @voice_cat.max_frame = length / 3
    @voice_niko = Audio.create_sound("Audio/.cats", false, Audio.music_group)
    @voice_niko.start_frame = length / 3
    @voice_niko.max_frame = length / 3 * 2

    @miss_sound = Audio.create_sound("Audio/SE/shatter")

    @viewport_ui = Viewport.new(0, 0, Graphics.width, Graphics.height)
    @viewport_ui.ox = -Graphics.width / 2
    @viewport_ui.oy = -Graphics.height / 2
    @spritesheet = RPG::Cache.misc(".cats")

    @cat_flying_arrows = []
    @player_flying_arrows = []
    @cat_tails = []
    @cat_tails_end = []
    @player_tails = []
    @player_tails_end = []
    @cat_arrows = []
    @player_arrows = []
    @events = []
    for i in 0..3
      cat_arrow = Sprite.new(@viewport_ui)
      cat_arrow.bitmap = Bitmap.new(ARROW_SIZE, ARROW_SIZE)
      cat_arrow.bitmap.stretch_blt(Rect.new(0, 0, ARROW_SIZE, ARROW_SIZE), @spritesheet, SPRITES[CAT_ARROWS_SPRITES[:base][i]])
      cat_arrow.zoom_x = cat_arrow.zoom_y = ARROW_SCALE
      cat_arrow.x = ARROWS_SIDES_MARGIN + (ARROW_SIZE * ARROW_SCALE + ARROWS_PADDING) * i - Graphics.width / 2
      cat_arrow.y = ARROWS_TOP_MARGIN - Graphics.height / 2
      @cat_arrows << cat_arrow

      player_arrow = Sprite.new(@viewport_ui)
      player_arrow.bitmap = Bitmap.new(ARROW_SIZE, ARROW_SIZE)
      player_arrow.bitmap.stretch_blt(Rect.new(0, 0, ARROW_SIZE, ARROW_SIZE), @spritesheet, SPRITES[PLAYER_ARROWS_SPRITES[:base][i]])
      player_arrow.zoom_x = player_arrow.zoom_y = ARROW_SCALE
      player_arrow.x = Graphics.width / 2 - ARROWS_SIDES_MARGIN + (ARROW_SIZE * ARROW_SCALE + ARROWS_PADDING) * i - (ARROW_SIZE * ARROW_SCALE * 4 + ARROWS_PADDING * 3)
      player_arrow.y = ARROWS_TOP_MARGIN - Graphics.height / 2
      @player_arrows << player_arrow
    end

    @simple_bitmap = Bitmap.new(1, 1)
    @simple_bitmap.fill_rect(0, 0, 2, 2, Color.new(255, 255, 255))

    @hp_bg = Sprite.new(@viewport_ui)
    @hp_bg.bitmap = @simple_bitmap
    @hp_bg.zoom_x = HP_WIDTH
    @hp_bg.zoom_y = HP_HEIGHT
    @hp_bg.modulate = Color.new(0, 12, 24)
    @hp_bg.x = -HP_WIDTH / 2
    @hp_bg.y = -HP_HEIGHT / 2 + Graphics.height / 2 - ARROWS_SIDES_MARGIN

    @hp_blue = Sprite.new(@viewport_ui)
    @hp_blue.bitmap = @simple_bitmap
    @hp_blue.zoom_x = HP_WIDTH - 4
    @hp_blue.zoom_y = HP_HEIGHT - 4
    @hp_blue.modulate = Color.new(72, 73, 200)
    @hp_blue.x = -HP_WIDTH / 2 - 2
    @hp_blue.y = -HP_HEIGHT / 2 - 2 + Graphics.height / 2 - ARROWS_SIDES_MARGIN
    @hp_blue.z = @hp_bg.z + 1

    @hp_white = Sprite.new(@viewport_ui)
    @hp_white.bitmap = @simple_bitmap
    @hp_white.zoom_x = HP_WIDTH * 0.5 - 2
    @hp_white.zoom_y = HP_HEIGHT - 4
    @hp_white.x = -HP_WIDTH / 2 - 2
    @hp_white.y = -HP_HEIGHT / 2 - 2 + Graphics.height / 2 - ARROWS_SIDES_MARGIN
    @hp_white.z = @hp_bg.z + 2

    @hp_icons = Sprite.new(@viewport_ui)
    @hp_icons.bitmap = Bitmap.new(128, 64)
    @hp_icons.ox = 64
    @hp_icons.oy = 64
    @hp_icons.y = Graphics.height / 2 - ARROWS_SIDES_MARGIN + HP_HEIGHT / 2 - 6
    @hp_icons.z = @hp_white.z + 1

    @rotate_power = 0
    @reverse = false;

    self.hp = 50

    @bst_debug = Sprite.new(@viewport_ui)
    @bst_debug.bitmap = Bitmap.new(300, 40)

    @total_time = 0
    @bump_timeout = 0
    @icons_bump_timeout = 0
    @miss_timeout = 0

    make_mapping

    # Execute transition
    Graphics.transition(40)

    @instruments.play
    @voice_cat.play
    @voice_niko.play
    
    # Main loop
    while true
      # Update game screen
      Graphics.update
      # Update input information
      Input.update
      # only for legacy funcs
      Audio.update
      # Frame update
      update
      # Abort loop if screen is changed
      if $scene != self
        break
      end
    end
    Graphics.freeze
  end

  def update
    @total_time += 1
    @miss_timeout -= 1
    @voice_niko.volume = @miss_timeout <= 0 ? 1.0 : 0.0

    @events.each do |event|
      if @total_time * ARROWS_SPEED >= event[1]
        instance_exec(&event[0])
        @events.delete(event)
      end
    end

    # fl studio b:s:t :3
    beat = (@total_time / BPM_TIME / 2).to_i + 1
    step = (@total_time / BPM_TIME * 15 / 2).to_i % 15 + 1
    tick = (@total_time / BPM_TIME * 24 * 15 / 2).to_i % 24
    
    @bst_debug.bitmap.clear
    @bst_debug.bitmap.draw_text(Rect.new(0, 0, 300, 40), "#{beat} : #{step} : #{tick}")

    @bump_timeout -= 1
    @icons_bump_timeout -= 1
    if @bump_timeout <= 0
      @bump_timeout += BPM_TIME
      @viewport_ui.scale_y = @viewport_ui.scale_x = BUMP_SCALE
    end
    if @icons_bump_timeout <= 0
      @icons_bump_timeout += BPM_TIME / 2.0
      @hp_icons.zoom_x = @hp_icons.zoom_y = ICONS_BUMP_SCALE
      @viewport_ui.rotation = @reverse ? -@rotate_power : @rotate_power
      @reverse = !@reverse
    end
    @viewport_ui.scale_x = @viewport_ui.scale_y = @viewport_ui.scale_x * (1.0 - BUMP_RETURN_SPEED) + BUMP_RETURN_SPEED
    @hp_icons.zoom_x = @hp_icons.zoom_y = @hp_icons.zoom_x * (1.0 - ICONS_BUMP_RETURN_SPEED) + ICONS_BUMP_RETURN_SPEED
    @viewport_ui.rotation = @viewport_ui.rotation * (1.0 - BUMP_RETURN_SPEED)

    # Cat Arrows
    i = 0
    while i < @cat_flying_arrows.length
      arrow = @cat_flying_arrows[i]
      arrow[0].y -= ARROWS_SPEED
      if arrow[0].y < ARROWS_TOP_MARGIN - Graphics.height / 2
        arrow[0].dispose
        @cat_flying_arrows.delete(arrow)
        i -= 1
      end
      i += 1
    end
    # Cat Arrows Tails (kolbaski)
    i = 0
    while i < @cat_tails.length
      arrow = @cat_tails[i]
      arrow[0].y -= ARROWS_SPEED
      if arrow[0].y < ARROWS_TOP_MARGIN - Graphics.height / 2 + HALF_OF_ARROW
        delta = (ARROWS_TOP_MARGIN - Graphics.height / 2 + HALF_OF_ARROW) - arrow[0].y
        arrow[0].y += delta
        arrow[0].zoom_y -= pixels_to_zoom(arrow[0], delta)
        if (arrow[0].zoom_y <= 0)
          arrow[0].dispose
          @cat_tails.delete(arrow)
          i -= 1
        end
      end
      i += 1
    end
    # Cat Tails ends
    i = 0
    while i < @cat_tails_end.length
      arrow = @cat_tails_end[i]
      arrow[0].y -= ARROWS_SPEED
      if arrow[0].y < ARROWS_TOP_MARGIN - Graphics.height / 2 + HALF_OF_ARROW
        arrow[0].dispose
        @cat_tails_end.delete(arrow)
        i -= 1
      end
      i += 1
    end
    # Player Arrows
    i = 0
    while i < @player_flying_arrows.length
      arrow = @player_flying_arrows[i]
      arrow[0].y -= ARROWS_SPEED
      if arrow[0].y < - Graphics.height / 2 - ARROW_SIZE * ARROW_SCALE
        miss
        arrow[0].dispose
        @player_flying_arrows.delete(arrow)
        i -= 1
      end
      i += 1
    end
    # Player Arrows Tails (kolbaski)
    i = 0
    while i < @player_tails.length
      arrow = @player_tails[i]
      arrow[0].y -= ARROWS_SPEED
      if arrow[0].y < -Graphics.height / 2
        delta = -Graphics.height / 2 - arrow[0].y
        arrow[0].y += delta
        arrow[0].zoom_y -= pixels_to_zoom(arrow[0], delta)
        miss(0.5)
        if (arrow[0].zoom_y <= 0)
          arrow[0].dispose
          @player_tails.delete(arrow)
          i -= 1
        end
      end
      i += 1
    end
    # Player Tails ends
    i = 0
    while i < @player_tails_end.length
      arrow = @player_tails_end[i]
      arrow[0].y -= ARROWS_SPEED
      if arrow[0].y < - Graphics.height / 2 - ARROW_SIZE * ARROW_SCALE / 2
        miss(0.5)
        arrow[0].dispose
        @player_tails_end.delete(arrow)
        i -= 1
      end
      i += 1
    end

    # player
    for dir in 1..4
      mapped = map_input(dir * 2)
      is_pressed = Input.press?(dir * 2)
      if @arrows_states[mapped] != is_pressed
        @arrows_states[mapped] = is_pressed
        @player_arrows[mapped].bitmap.clear
        @player_arrows[mapped].bitmap.stretch_blt(Rect.new(0, 0, ARROW_SIZE, ARROW_SIZE), @spritesheet, SPRITES[PLAYER_ARROWS_SPRITES[@arrows_states[mapped] ? :pressed : :base][mapped]])
      end

      if is_pressed
        
        doing = true
        i = 0
        while doing && i < @player_tails.length
          arrow = @player_tails[i]
          if arrow[1] == mapped
            if arrow[0].y > ARROWS_TOP_MARGIN - Graphics.height / 2
              if arrow[0].y < ARROWS_TOP_MARGIN - Graphics.height / 2 + HALF_OF_ARROW
                delta = (ARROWS_TOP_MARGIN - Graphics.height / 2 + HALF_OF_ARROW) - arrow[0].y
                arrow[0].y += delta
                arrow[0].zoom_y -= pixels_to_zoom(arrow[0], delta)
                self.hp += 0.5
                if (arrow[0].zoom_y <= 0)
                  arrow[0].dispose
                  @player_tails.delete(arrow)
                end
              end
              doing = false
            end
          end
          i += 1
        end
        
        doing = true
        i = 0
        while doing && i < @player_tails_end.length
          arrow = @player_tails_end[i]
          if arrow[1] == mapped
            if arrow[0].y > ARROWS_TOP_MARGIN - Graphics.height / 2
              if arrow[0].y < ARROWS_TOP_MARGIN - Graphics.height / 2 + HALF_OF_ARROW
                arrow[0].dispose
                @player_tails_end.delete(arrow)
                self.hp += 0.5
              end
            end
            doing = false
          end
          i += 1
        end
      end
      
      if Input.trigger?(dir * 2)
        doing = true
        i = 0
        while doing && i < @player_flying_arrows.length
          arrow = @player_flying_arrows[i]
          if arrow[1] == mapped
            if arrow[0].y > ARROWS_TOP_MARGIN - Graphics.height / 2 - ARROWS_ALLOW_ZONE_SIZE
              if arrow[0].y < ARROWS_TOP_MARGIN - Graphics.height / 2 + ARROWS_ALLOW_ZONE_SIZE
                arrow[0].dispose
                @player_flying_arrows.delete(arrow)
                self.hp += 5
                @miss_sound.fade_out(0.2)
                @miss_timeout = 0
              else
                miss
              end
              doing = false
            end
          end
          i += 1
        end
      end
    end
  end

  def miss(hp = 10)
    self.hp -= hp
    @miss_timeout = MISS_TIME
    if !@miss_sound.playing || @miss_sound.position > 0.1
      @miss_sound.play(0, 0.6, rand(75..125) / 100.0)
    end
  end

  def hp
    @hp
  end
  def hp=(val)
    @hp = val.clamp(0, 100)
    @hp_white.zoom_x = (HP_WIDTH - 4) * (100 - @hp) / 100.0
    @hp_icons.x = -HP_WIDTH / 2 - 2 + (HP_WIDTH - 4) * (100 - @hp) / 100.0
    @hp_icons.bitmap.clear
    if @hp <= 25
      @hp_icons.bitmap.stretch_blt(Rect.new(64, 0, 64, 64), @spritesheet, SPRITES[:hpbar_player_lose])
      @hp_icons.bitmap.stretch_blt(Rect.new(0,  0, 64, 64), @spritesheet, SPRITES[:hpbar_cat_win])
    elsif @hp >= 75
      @hp_icons.bitmap.stretch_blt(Rect.new(64, 0, 64, 64), @spritesheet, SPRITES[:hpbar_player_win])
      @hp_icons.bitmap.stretch_blt(Rect.new(0,  0, 64, 64), @spritesheet, SPRITES[:hpbar_cat_lose])
    else
      @hp_icons.bitmap.stretch_blt(Rect.new(64, 0, 64, 64), @spritesheet, SPRITES[:hpbar_player_normal])
      @hp_icons.bitmap.stretch_blt(Rect.new(0,  0, 64, 64), @spritesheet, SPRITES[:hpbar_cat_normal])
    end
  end

  def map_input(input)
    case input
    when 2
      1
    when 4
      0
    when 6
      3
    when 8
      2
    end
  end

  def make_mapping
    @cat_flying_arrows = []
    @player_flying_arrows = []
    #beat = (@total_time / BPM_TIME / 2).to_i + 1
    #step = (@total_time / BPM_TIME * 15 / 2).to_i % 15 + 1
    #tick = (@total_time / BPM_TIME * 24 * 15 / 2).to_i % 24
    i = 0
    while i < SONG_MAPPING.length
      line_data = SONG_MAPPING[i]
      beat = line_data[1]
      step = line_data[2]
      tick = line_data[3]
      lenght = bst2time((line_data[4] || 0) + 1, (line_data[5] || 0) + 1, line_data[6] || 0) - HALF_OF_ARROW
      for dir in 0..3
        cat_arrow_x = ARROWS_SIDES_MARGIN + (ARROW_SIZE * ARROW_SCALE + ARROWS_PADDING) * dir - Graphics.width / 2
        player_arrow_x = Graphics.width / 2 - ARROWS_SIDES_MARGIN + (ARROW_SIZE * ARROW_SCALE + ARROWS_PADDING) * dir - (ARROW_SIZE * ARROW_SCALE * 4 + ARROWS_PADDING * 3)

        if line_data[0][dir] == '#'
          arrow = make_arrow(dir, true)
          arrow.x = cat_arrow_x
          arrow.y = ARROWS_TOP_MARGIN - Graphics.height / 2 + bst2time(beat, step, tick) - 32

          @cat_flying_arrows << [arrow, dir]
          
          if lenght > 0
            tail = make_tail(dir, true)
            tail.x = cat_arrow_x
            tail.y = arrow.y + HALF_OF_ARROW

            tail.zoom_y = pixels_to_zoom(tail, lenght + HALF_OF_ARROW)

            @cat_tails << [tail, dir]

            tail_end_arrow = make_tail_end(dir, true)
            tail_end_arrow.x = cat_arrow_x
            tail_end_arrow.y = arrow.y + lenght + ARROW_SIZE * ARROW_SCALE
            @cat_tails_end << [tail_end_arrow, dir]
          end
        end

        if line_data[0][dir + 5] == '#'
          arrow = make_arrow(dir, false)
          arrow.x = player_arrow_x
          arrow.y = ARROWS_TOP_MARGIN - Graphics.height / 2 + bst2time(beat, step, tick) - 32

          @player_flying_arrows << [arrow, dir]

          if lenght > 0
            tail = make_tail(dir, false)
            tail.x = player_arrow_x
            tail.y = arrow.y + HALF_OF_ARROW

            tail.zoom_y = pixels_to_zoom(tail, lenght + HALF_OF_ARROW)

            @player_tails << [tail, dir]

            tail_end_arrow = make_tail_end(dir, false)
            tail_end_arrow.x = player_arrow_x
            tail_end_arrow.y = arrow.y + lenght + ARROW_SIZE * ARROW_SCALE
            @player_tails_end << [tail_end_arrow, dir]
          end
        end
      end

      if line_data[7] != nil
        @events << [line_data[7], bst2time(beat, step, tick)]
      end

      i += 1
    end
  end

  def pixels_to_zoom(sprite, pixels, voh = false) # false - vertical, true - horizontal
      pixels.to_f / (voh ? sprite.bitmap.width.to_f : sprite.bitmap.height.to_f)
  end

  def make_arrow(direction, meow)
    arrow = Sprite.new(@viewport_ui)
    arrow.bitmap = Bitmap.new(ARROW_SIZE, ARROW_SIZE)
    arrow.bitmap.stretch_blt(Rect.new(0, 0, ARROW_SIZE, ARROW_SIZE), @spritesheet, SPRITES[(meow ? CAT_ARROWS_SPRITES : PLAYER_ARROWS_SPRITES)[:target][direction]])
    arrow.zoom_x = arrow.zoom_y = ARROW_SCALE
    arrow.z = 20
    arrow
  end

  def make_tail(direction, meow)
    arrow = Sprite.new(@viewport_ui)
    arrow.bitmap = Bitmap.new(ARROW_SIZE, ARROW_SIZE / 2)
    arrow.bitmap.stretch_blt(Rect.new(0, 0, ARROW_SIZE, ARROW_SIZE / 2), @spritesheet, SPRITES[(meow ? CAT_ARROWS_SPRITES : PLAYER_ARROWS_SPRITES)[:tail][direction]])
    arrow.zoom_x = arrow.zoom_y = ARROW_SCALE
    arrow.z = 18
    arrow
  end

  def make_tail_end(direction, meow)
    arrow = Sprite.new(@viewport_ui)
    arrow.bitmap = Bitmap.new(ARROW_SIZE, ARROW_SIZE / 2)
    arrow.bitmap.stretch_blt(Rect.new(0, 0, ARROW_SIZE, ARROW_SIZE / 2), @spritesheet, SPRITES[(meow ? CAT_ARROWS_SPRITES : PLAYER_ARROWS_SPRITES)[:tail_end][direction]])
    arrow.zoom_x = arrow.zoom_y = ARROW_SCALE
    arrow.z = 19
    arrow
  end

  def bst2time(b, s, t)
    (b - 1) * 2 * BPM_TIME * ARROWS_SPEED + 
    (s - 1) / 15.0 * 2 * BPM_TIME * ARROWS_SPEED + 
    t / 24.0 / 15.0 * 2 * BPM_TIME * ARROWS_SPEED
  end

  SONG_MAPPING = [            # Length
    # arrows,      B,  S,  T, | B, S, T | proc
    ["#   |    ",  5,  1,  0,   0, 1, 0],
    ["  # |    ",  5,  3,  0,   0, 1, 0],
    [" #  |    ",  5,  5,  0,   0, 1, 0],
    ["   #|    ",  5,  9,  0,   0, 1, 0],
    ["  # |    ",  5, 13,  0,   0, 1, 0],
    ["#   |    ",  6,  1,  0,   0, 1, 0],
    [" #  |    ",  6,  3,  0,   0, 1, 0],
    ["  # |    ",  6,  5,  0,   0, 1, 0],
    ["  # |    ",  6,  9,  0,   0, 1, 0],
    ["#   |    ",  6, 13,  0,   0, 1, 0],
    [" #  |    ",  7,  1,  0,   0, 1, 0],
    ["   #|    ",  7,  3,  0,   0, 1, 0],
    ["  # |    ",  7,  5,  0,   0, 1, 0],
    ["#   |    ",  7,  9,  0,   0, 1, 0],
    ["#   |    ",  7, 13,  0,   0, 1, 0],
    ["#   |    ",  8,  1,  0,   0, 1, 0],
    [" #  |    ",  8,  3,  0,   0, 1, 0],
    [" #  |    ",  8,  6,  0,   0, 1, 0],
    ["  # |    ",  8,  8,  0,   0, 1, 0],
    [" #  |    ",  8, 11,  0,   0, 1, 0],
    ["   #|    ",  8, 13,  0,   0, 1, 0],

    ["    |#   ",  9,  1,  0,   0, 1, 0],
    ["    |  # ",  9,  3,  0,   0, 1, 0],
    ["    | #  ",  9,  5,  0,   0, 1, 0],
    ["    |   #",  9,  9,  0,   0, 1, 0],
    ["    |  # ",  9, 13,  0,   0, 1, 0],
    ["    |#   ", 10,  1,  0,   0, 1, 0],
    ["    | #  ", 10,  3,  0,   0, 1, 0],
    ["    |  # ", 10,  5,  0,   0, 1, 0],
    ["    |  # ", 10,  9,  0,   0, 1, 0],
    ["    |#   ", 10, 13,  0,   0, 1, 0],
    ["    | #  ", 11,  1,  0,   0, 1, 0],
    ["    |   #", 11,  3,  0,   0, 1, 0],
    ["    |  # ", 11,  5,  0,   0, 1, 0],
    ["    |#   ", 11,  9,  0,   0, 1, 0],
    ["    |#   ", 11, 13,  0,   0, 1, 0],
    ["    |#   ", 12,  1,  0,   0, 1, 0],
    ["    | #  ", 12,  3,  0,   0, 1, 0],
    ["    | #  ", 12,  5,  0],
    ["    | #  ", 12,  6,  0],
    ["    | #  ", 12,  7,  0],
    ["    |  # ", 12,  8,  0],
    ["    | #  ", 12,  9,  0],
    ["    |   #", 12, 10,  0],
    ["    | #  ", 12, 11,  0,   0, 1, 0],
    ["    |#   ", 12, 13,  0],
    ["    |#   ", 12, 15,  0],
   #["    |#   ", 12, 15, 12],
    ["    |#   ", 12, 16,  0],
   #["    |#   ", 12, 16, 12],
   #["    |#   ", 13,  1,  0],
    
    ["#   |    ", 13,  1,  0,   0, 0, 0,    proc { @rotate_power = 1 }],
    [" #  |    ", 13,  2,  0],
    ["  # |    ", 13,  3,  0],
    [" #  |    ", 13,  4,  0],
    ["#   |    ", 13,  5,  0,   0, 1, 0],
    ["   #|    ", 13,  7,  0],
    ["#   |    ", 13,  8,  0],
    ["  # |    ", 13,  9,  0],
    ["#   |    ", 13, 10,  0],
    ["  # |    ", 13, 11,  0],
    ["   #|    ", 13, 12,  0],
    [" #  |    ", 13, 13,  0,   0, 1, 0],
    ["  # |    ", 13, 15,  0],
    ["   #|    ", 14,  1,  0],
    ["  # |    ", 14,  2,  0],
    ["#   |    ", 14,  3,  0],
    [" #  |    ", 14,  4,  0],
    ["#   |    ", 14,  5,  0,   0, 1, 0],
    ["  # |    ", 14,  7,  0],
    ["#   |    ", 14,  8,  0],
    ["  # |    ", 14,  9,  0],
    ["   #|    ", 14, 10,  0],
    ["  # |    ", 14, 11,  0],
    ["   #|    ", 14, 12,  0],
    [" #  |    ", 14, 13,  0,   0, 1, 0],
    ["#   |    ", 14, 15,  0],
    ["  # |    ", 15,  1,  0],
    ["   #|    ", 15,  2,  0],
    [" #  |    ", 15,  3,  0],
    ["#   |    ", 15,  4,  0],
    ["#   |    ", 15,  5,  0,   0, 1, 0],
    ["  # |    ", 15,  7,  0],
    ["   #|    ", 15,  8,  0],
    [" #  |    ", 15,  9,  0],
    ["  # |    ", 15, 10,  0],
    [" #  |    ", 15, 11,  0],
    ["  # |    ", 15, 12,  0],
    ["   #|    ", 15, 13,  0,   0, 1, 0],
    ["#   |    ", 15, 15,  0,   0, 1, 0],
    ["   #|    ", 16,  1,  0],
    ["  # |    ", 16,  2,  0],
    [" #  |    ", 16,  3,  0],
    ["#   |    ", 16,  4,  0],
    [" #  |    ", 16,  5,  0],
    ["  # |    ", 16,  7,  0],
    [" #  |    ", 16,  8,  0],
    ["#   |    ", 16,  9,  0],
    ["   #|    ", 16, 10,  0],
    ["  # |    ", 16, 11,  0],
    [" #  |    ", 16, 12,  0],
    ["   #|    ", 16, 13,  0,   0, 1, 0],
    ["   #|    ", 16, 15,  0,   0, 1, 0],
    
    ["    |#   ", 17,  1,  0],
    ["    | #  ", 17,  2,  0],
    ["    |  # ", 17,  3,  0],
    ["    | #  ", 17,  4,  0],
    ["    |#   ", 17,  5,  0,   0, 1, 0],
    ["    |   #", 17,  7,  0],
    ["    |#   ", 17,  8,  0],
    ["    |  # ", 17,  9,  0],
    ["    |#   ", 17, 10,  0],
    ["    |  # ", 17, 11,  0],
    ["    |   #", 17, 12,  0],
    ["    | #  ", 17, 13,  0,   0, 1, 0],
    ["    |  # ", 17, 15,  0],
    ["    |   #", 18,  1,  0],
    ["    |  # ", 18,  2,  0],
    ["    |#   ", 18,  3,  0],
    ["    | #  ", 18,  4,  0],
    ["    |#   ", 18,  5,  0,   0, 1, 0],
    ["    |  # ", 18,  7,  0],
    ["    |#   ", 18,  8,  0],
    ["    |  # ", 18,  9,  0],
    ["    |   #", 18, 10,  0],
    ["    |  # ", 18, 11,  0],
    ["    |   #", 18, 12,  0],
    ["    | #  ", 18, 13,  0,   0, 1, 0],
    ["    |#   ", 18, 15,  0],
    ["    |  # ", 19,  1,  0],
    ["    |   #", 19,  2,  0],
    ["    | #  ", 19,  3,  0],
    ["    |#   ", 19,  4,  0],
    ["    |#   ", 19,  5,  0,   0, 1, 0],
    ["    |  # ", 19,  7,  0],
    ["    |   #", 19,  8,  0],
    ["    | #  ", 19,  9,  0],
    ["    |  # ", 19, 10,  0],
    ["    | #  ", 19, 11,  0],
    ["    |  # ", 19, 12,  0],
    ["    |   #", 19, 13,  0,   0, 1, 0],
    ["    |#   ", 19, 15,  0,   0, 1, 0],
    ["    |   #", 20,  1,  0],
    ["    |  # ", 20,  2,  0],
    ["    | #  ", 20,  3,  0],
    ["    |#   ", 20,  4,  0],
    ["    | #  ", 20,  5,  0,   0, 1, 0],
    ["    |  # ", 20,  7,  0],
    ["    | #  ", 20,  8,  0],
    ["    |#   ", 20,  9,  0],
    ["    |   #", 20, 10,  0],
    ["    |  # ", 20, 11,  0],
    ["    | #  ", 20, 12,  0],
    ["    |   #", 20, 13,  0,   0, 1, 0],
    
    [" #  |    ", 21,  1,  0,   0, 0, 0,    proc { @rotate_power = 0 }],
    ["  # |    ", 21,  2,  0],
    ["#   |    ", 21,  3,  0],
    ["# # |    ", 21,  5,  0],
    [" # #|    ", 21,  7,  0],
    ["# # |    ", 21,  9,  0],
    ["  # |    ", 21, 11,  0],
    [" #  |    ", 21, 12,  0],
    ["#   |    ", 21, 13,  0],
    ["   #|    ", 22,  1,  0],
    ["  # |    ", 22,  2,  0],
    ["#   |    ", 22,  3,  0],
    ["  ##|    ", 22,  5,  0],
    ["# # |    ", 22,  7,  0],
    [" # #|    ", 22,  9,  0],
    [" #  |    ", 22, 11,  0],
    ["#   |    ", 22, 13,  0],
    [" #  |    ", 23,  1,  0],
    ["#   |    ", 23,  2,  0],
    ["  # |    ", 23,  3,  0],
    [" # #|    ", 23,  5,  0],
    ["# # |    ", 23,  7,  0],
    [" # #|    ", 23,  9,  0],
    ["  # |    ", 23, 11,  0],
    [" #  |    ", 23, 13,  0],
    ["   #|    ", 24,  1,  0],
    ["  # |    ", 24,  2,  0],
    [" #  |    ", 24,  3,  0],
    ["#  #|    ", 24,  5,  0],
    ["  ##|    ", 24,  7,  0],
    [" # #|    ", 24,  9,  0],
    ["   #|    ", 24, 11,  0],
    ["  # |    ", 24, 13,  0],
    
    ["    | #  ", 25,  1,  0],
    ["    |  # ", 25,  2,  0],
    ["    |#   ", 25,  3,  0],
    ["    |# # ", 25,  5,  0],
    ["    | # #", 25,  7,  0],
    ["    |# # ", 25,  9,  0],
    ["    |  # ", 25, 11,  0],
    ["    | #  ", 25, 12,  0],
    ["    |#   ", 25, 13,  0],
    ["    |   #", 26,  1,  0],
    ["    |  # ", 26,  2,  0],
    ["    |#   ", 26,  3,  0],
    ["    |  ##", 26,  5,  0],
    ["    |# # ", 26,  7,  0],
    ["    | # #", 26,  9,  0],
    ["    | #  ", 26, 11,  0],
    ["    |#   ", 26, 13,  0],
    ["    | #  ", 27,  1,  0],
    ["    |#   ", 27,  2,  0],
    ["    |  # ", 27,  3,  0],
    ["    | # #", 27,  5,  0],
    ["    |# # ", 27,  7,  0],
    ["    | # #", 27,  9,  0],
    ["    |  # ", 27, 11,  0],
    ["    | #  ", 27, 13,  0],
    ["    |   #", 28,  1,  0],
    ["    |  # ", 28,  2,  0],
    ["    | #  ", 28,  3,  0],
    ["    |#  #", 28,  5,  0],
    ["    |  ##", 28,  7,  0],
    ["    |   #", 28,  9,  0],
    ["    |#  #", 28, 11,  0],
    ["    | ## ", 28, 13,  0],

    ["#   |    ", 29,  1,  0,   0, 1, 0],
    ["  # |    ", 29,  3,  0,   0, 1, 0],
    [" #  |    ", 29,  5,  0,   0, 1, 0],
    ["   #|    ", 29,  9,  0,   0, 1, 0],
    ["  # |    ", 29, 13,  0,   0, 1, 0],
    ["#   |    ", 30,  1,  0,   0, 1, 0],
    [" #  |    ", 30,  3,  0,   0, 1, 0],
    ["  # |    ", 30,  5,  0,   0, 1, 0],
    ["  # |    ", 30,  9,  0,   0, 1, 0],
    ["#   |    ", 30, 13,  0,   0, 1, 0],
    [" #  |    ", 31,  1,  0,   0, 1, 0],
    ["   #|    ", 31,  3,  0,   0, 1, 0],
    ["  # |    ", 31,  5,  0,   0, 1, 0],
    ["#   |    ", 31,  9,  0,   0, 1, 0],
    ["#   |    ", 31, 13,  0,   0, 1, 0],
    ["#   |    ", 32,  1,  0,   0, 1, 0],
    [" #  |    ", 32,  3,  0,   0, 1, 0],
    [" #  |    ", 32,  6,  0,   0, 1, 0],
    ["  # |    ", 32,  8,  0,   0, 1, 0],
    [" #  |    ", 32, 11,  0,   0, 1, 0],
    ["   #|    ", 32, 13,  0,   0, 1, 0],

    ["    |#   ", 33,  1,  0,   0, 1, 0],
    ["    |  # ", 33,  3,  0,   0, 1, 0],
    ["    | #  ", 33,  5,  0,   0, 1, 0],
    ["    |   #", 33,  9,  0,   0, 1, 0],
    ["    |  # ", 33, 13,  0,   0, 1, 0],
    ["    |#   ", 34,  1,  0,   0, 1, 0],
    ["    | #  ", 34,  3,  0,   0, 1, 0],
    ["    |  # ", 34,  5,  0,   0, 1, 0],
    ["    |  # ", 34,  9,  0,   0, 1, 0],
    ["    |#   ", 34, 13,  0,   0, 1, 0],
    ["    | #  ", 35,  1,  0,   0, 1, 0],
    ["    |   #", 35,  3,  0,   0, 1, 0],
    ["    |  # ", 35,  5,  0,   0, 1, 0],
    ["    |#   ", 35,  9,  0,   0, 1, 0],
    ["    |#   ", 35, 13,  0,   0, 1, 0],
    ["# # |#   ", 36,  1,  0,   0, 1, 0],
    ["    | #  ", 36,  3,  0,   0, 1, 0],
    [" # #|    ", 36,  4,  0,   0, 1, 0],
    ["    | #  ", 36,  6,  0,   0, 1, 0],
    ["    |  # ", 36,  8,  0,   0, 1, 0],
    ["  ##|    ", 36,  9,  0,   0, 1, 0],
    ["    | #  ", 36, 11,  0,   0, 1, 0],
    ["#  #|   #", 36, 13,  0,   0, 1, 0],

    ["   #|   #", 37,  1,  0,   0, 0, 0,    proc { @rotate_power = 1 }],
    [" #  | #  ", 37,  2,  0],
    ["#   |#   ", 37,  3,  0],
    [" #  | #  ", 37,  4,  0],
    ["   #|   #", 37,  6,  0],
    ["  # |  # ", 37,  7,  0],
    [" #  | #  ", 37,  8,  0],
    ["  # |  # ", 37,  9,  0],
    ["   #|   #", 37, 10,  0],
    [" #  | #  ", 37, 11,  0],
    ["  # |  # ", 37, 12,  0],
    ["   #|   #", 37, 13,  0,   0, 1, 0],
    [" #  | #  ", 37, 15,  0],
  ]
end

# ████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████
# ████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████
# ████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████
# ████████████        ██████████████████████████████████  ██████████████████████████████████████████████████████████    ████████    ██████
# ██████████  ████████  ████████████  ████████████████  ██  ████████████████████████████████████████████████████████      ████      ██████
# ████████  ████████████████  ██████  ██████████    ██████  ██████████████████████████████████████████████████████████            ████████
# ████████  ██████████████  ██  ██      ████████    ████  ██████████████████████████████████████████████████████████████        ██████████
# ████████  ██████████████  ██  ████  ████████████████████  ████████████████████████████████████████████████████████████        ██████████
# ████████  ██████████████  ██  ████  ████████████████████  ██████████████████████████████████████████████████████████            ████████
# ██████████  ████████  ██  ██  ████  ██  ████████  ██  ██  ████████████████████████████████████████████████████████      ████      ██████
# ████████████        ██████  ██  ████  ██████████  ████  ██████████████████████████████████████████████████████████    ████████    ██████
# ██████████████████████████████████████████████  ████████████████████████████████████████████████████████████████████████████████████████
# ████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████
# ████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████
# ████                                                                                                                                ████
# ████                                                  ██████                                                                        ████
# ████                                                  ████████                                                                      ████
# ████                                                  ████████                                                                      ████
# ████                                                  ██████████                                                                    ████
# ████                                                  ██████████                                                                    ████
# ████                                                  ████████████                                                                  ████
# ████                                                  ██████████████                                                                ████
# ████                                                  ██████████████                                                                ████
# ████                                                  ████████████████                                                              ████
# ████                                                  ████████████████                                                ██████        ████
# ████                                                  ██████████████████                                          ████████          ████
# ████                                                  ██████████████████                                      ████████████          ████
# ████                                                  ████████████████████                              ████████████████            ████
# ████                                                  ██████████████████████                        ████████████████████            ████
# ████                                                  ██████████████████████                    ██████████████████████              ████
# ████                                                  ████████████████████████              ██████████████████████████              ████
# ████                                                  ████████████████████████          ████████████████████████████                ████
# ████                                                  ██████████████████████████    ████████████████████████████████                ████
# ████                                                    ████████████████████████    ████████████████████████████████                ████
# ████                                                    ██████████████████████████  ████████████████████                            ████
# ████                                ████████████████    ██████████████████████████████████████████████████████                      ████
# ████                                ████████████████    ██████████████████████████████████████████████████████                      ████
# ████                                ████████████████    ██████████████████████████████████████████████████████                      ████
# ████                                ████████████████    ██████████████████████████████████████████████████████                      ████
# ████                                ████████████████    ██████████████████████████████████████████████████████                      ████
# ████                                ████████████████    ██████████████████████████████████████████████████████                      ████
# ████                                ████████████████    ██████████████████████████████████████████████████████                      ████
# ████                                ████████████████    ██████████████████████████████████████████████████████                      ████
# ████                                ████████████████    ██████    ████████████████████████████████████████████                      ████
# ████                                ████████████████          ████████████████████████████████████████████████                      ████
# ████                                ██████████████████████████████████████████████████████████████████████████                      ████
# ████                                ██████████████████████████████████████████████        ██████        ██████                      ████
# ████                                ██████████████████████████████████████████████      ████████      ████████                      ████
# ████                                ██████████████████████████████████████████████      ████████      ████████                      ████
# ████                                ██████████████████████████████████████████████      ████████      ████████                      ████
# ████                                ██████████████████████████████████████████████████████████████████████████                      ████
# ████                                ██████████████████████████████████████████████████████████████████████████                      ████
# ████                                ██████████████████████████████████████████████████████████████████████████                      ████
# ████                                ██████████████████████████████████████████████████    ██████    ██████████                      ████
# ████                                ████████████████████████████████████████████████    ████    ██    ████████                      ████
# ████                                ████████████████████████████████████████████████                  ████████                      ████
# ████                                ██████████████████████████████████████████████████      ████    ██████████                      ████
# ████                                ██████████████████████████████████████████████████████████████████████████                      ████
# ████                                ██████████████████████████████████████████████████████████████████████████                      ████
# ████                                ██████████████████████████████████████████████████████████████████████████                      ████
# ████                                ██████████████████████████████████████████████████████████████████████████                      ████
# ████                    ████                ██████████████████████████████████████████████████████████████████                      ████
# ████                ██████████                                                                                                      ████
# ████              ██████████████                                                                                                    ████
# ████              ██████████████                  ████████████████████████████                                                      ████
# ████            ██████████████████              ████████████████████████████████████████████████████                                ████
# ████          ████████████████████              ████████████████████████████████████████████████████                                ████
# ████          ████████████████████              ██████████████████████████████████████████████████████                              ████
# ████          ████████████████████              ██████████████████████████████████████████████████████    ██                        ████
# ████        ██████████████████████            ████████████████████████████████████████████████████████    ████                      ████
# ████        ████████████████████                  ██████████████████████████████████████████████████████  ██████                    ████
# ████        ████████████████████      ██████████████████████████████████████████████████████████████████    ██████                  ████
# ████        ████████████████████      ████████████████████████████████████████████████████████████████████  ████████                ████
# ████      ████████████████████        ████████████████████████████████████████████████████████████████████    ██████                ████
# ████      ████████████████████      ████████████████████████████████████████████████████████████████████████  ████████              ████
# ████      ████████████████████      ████████████████████████████████████████████████████████████████████████    ██████              ████
# ████    ████████████████████        ██████████████████████████████████████████████████████████████████████████  ██████              ████
# ████    ████████████████████        ██████████████████████████████████████████████████████████████████████████    ██████            ████
# ████    ██████████████████          ████████████████████████████████████████████████████████████████████████████  ██████            ████
# ████    ██████████████████          ██████████████████████████████████████████████████████████████████████████    ████████          ████
# ████  ████████████████████          ████████████████████████████████████████████████████████████████████████    ██████████          ████
# ████  ████████████████████          ██████████████████████████████████████████████████████████████████████    ████████████          ████
# ████  ████████████████████          ██████████████████████████████████████████████████████████████████████  ████████████████        ████
# ████    ██████████████████            ██████████████████████████████████████████████████████████████████    ████████████████        ████
# ████    ██████████████████            ████████████████████████████████████████████████████████████████    ████████████████████      ████
# ████    ██████████████████            ██████████████████████████████████████████████████████████████    ████████████████████████    ████
# ████    ██████████████████            ████████████████████████████████████████████████████████████    ██████████████████████████    ████
# ████      ████████████████            ████████████████████████████  ██████████████████████████████    ████████████████████████████  ████
# ████      ██████████████████          ████████████████████████████  ██████████████████████████████      ██████████████████  ██████  ████
# ████      ██████████████████          ██████████████████████████  ████████████████████████████████          ████████████  ██████    ████
# ████      ██████████████████          ██████████████████████████  ██████████████████████████████          ██            ████████    ████
# ████        ████████████████          ██████████████████████████  ██████████████████████████████          ████████████████████      ████
# ████        ████████████████          ████████████████████████    ██████████████████████████████        ████████████████████████    ████
# ████        ████████████████          ████████████████████████    ██████████████████████████████        ████████████████████████    ████
# ████        ████████████████          ████████████████████████    ██████████████████████████████        ████████████████████████    ████
# ████          ████████████████          ██████████████████████    ██████████████████████████████      ████████████████████████      ████
# ████          ████████████████          ██████████████████████  ████████████████████████████████      ██████████████████████        ████
# ████          ████████████████          ██████████████████████  ████████████████████████████████    ██████████████████████          ████
# ████          ██████████████████        ██████████████████████  ████████████████████████████████    ██████████████████████          ████
# ████          ████████████████████      ██████████████████████  ██████████████████████████████    ██████████████████████            ████
# ████            ██████████████████      ██████████████████████  ████████████████████████████████    ██████████████████              ████
# ████              ██████████████████    ██████████████████████  ████████████████████████████████    ████████████████                ████
# ████                ████████████████    ██████████████████████  ██████████████████████████████████          ████████                ████
# ████                ████████████████  ████████████████████████  ████████████████████████████████████            ██                  ████
# ████                  ██████████████  ████████████████████████    ██████████████████████████████████    ██████                      ████
# ████                    ████████████  ████████████████████████  ██████████████████████████████████████    ████                      ████
# ████                      ██████████  ████████████████████████  ████████████████████████████████████████  ██                        ████
# ████                        ████████    ██████████████████████  ████████████████████████████████████████      ██                    ████
# ████                        ████████    ████████████████████    ██████████████████████████████████████████  ██████                  ████
# ████                          ██████    ████████████████████    ██████████████████████████████████████████  ██████                  ████
# ████                            ████    ████████████████████    ████████████████████████████████████████  ████████                  ████
# ████                              ██    ████████████████████    ████████████████████████████████████████  ████████                  ████
# ████                                    ████████████████████    ████████████████████████████████████████  ██████████                ████
# ████                                    ████████████████████  ██████████████████████████████████████████  ██████████                ████
# ████                                    ████████████████████  ██████████████████████████████████████████  ██████████                ████
# ████                                    ████████████████████  ██████████████████████████████████████████  ██████████                ████
# ████                                    ████████████████████  ████████████████████████████  ██████████████  ██████                  ████
# ████                                    ██████████████████    ████████████████████████████  ██████████████  ██████                  ████
# ████                                    ██████████████████    ████████████████████████████  ██████████████  ████                    ████
# ████                                    ██████████████████    ████████████████████████████  ████████████████                        ████
# ████                                      ████████████████  ██████████████████████████████    ██████████████████                    ████
# ████                                      ████████████████  ██████████████████████████████    ██████████████████                    ████
# ████                                      ████████████████  ██████████████████████████████    ████████████████                      ████
# ████                                      ████████████████  ██████████████████████████████    ████████████████                      ████
# ████                                      ████████████████  ██████████████████████████████    ████████████████                      ████
# ████                                      ████████████████  ██████████████████████████████    ████████████████                      ████
# ████                                      ██████████████    ██████████████████████████████    ████████████████                      ████
# ████                                      ██████████████      ████████████████████████████    ████████████████                      ████
# ████                                      ████████████████      ████████            ████████  ██████████████                        ████
# ████                                      ████████████████████    ████    ████  ████    ████  ██████████████                        ████
# ████                                      ██████████████████████  ██    ████  ████  ██    ██  ██████████████                        ████
# ████                                      ████████████████████████    ██              ██  ██  ██████████████                        ████
# ████                                      ██████████████████        ██  ██  ████  ██    ██    ██████████████                        ████
# ████                                      ████████████████  ██████    ████  ████  ████        ██████████████                        ████
# ████                                      ████████████████  ██████  ██                ██        ████████████                        ████
# ████                                      ████████████████  ██████    ████  ████  ████          ██████████                          ████
# ████                                      ████████████████  ██████  ██  ██  ████  ██    ██      ██████████                          ████
# ████                                      ██████████████████          ██            ████  ██    ██████████                          ████
# ████                                      ████████████████████████      ████  ████  ██    ██    ██████████                          ████
# ████                                          ██████████████████  ████    ████  ████    ████    ██████████                          ████
# ████                                                            ██████████          ████████    ██████████                          ████
# ████                                                    ████████████████████████████████████    ████████                            ████
# ████                                                      ████████████████████████████████████  ████████                            ████
# ████                                                      ████████████████████████████████████  ████████                            ████
# ████                                                      ████████████████████████████████████  ████████                            ████
# ████                                                        ██████████████████████████████████  ████████                            ████
# ████                                                        ██████████████████████████████████  ████████                            ████
# ████                                                          ████████████████████████████████    ████                              ████
# ████                                                          ████████████████████████████████    ██████                            ████
# ████                                                            ██████████████████████████████    ██████                            ████
# ████                                                            ██████████████████████████████    ██████                            ████
# ████                                                              ████████████████████████████    ██████                            ████
# ████                                                            ██████████████████████████████    ██████                            ████
# ████                                                            ██████████████████████████████    ██████                            ████
# ████                                                            ██████████████████████████████    ████                              ████
# ████                                                            ██████████████████████████████    ████                              ████
# ████                                                          ████████████████████████████████  ██████                              ████
# ████                                                          ████████████████████████████████  ██████                              ████
# ████                                                          ██████████████████████████████    ██████                              ████
# ████                                                          ██████████████████████████████  ██████                                ████
# ████                                                          ████████████████████████████    ██████                                ████
# ████                                                          ████████████████████████████  ████████                                ████
# ████                                                          ██████████████████████████    ████████                                ████
# ████                                                          ████████████████████████    ██████████                                ████
# ████                                                          ████████████████████████    ████████                                  ████
# ████                                                          ██████████████████████    ██████████                                  ████
# ████                                                          ██████████████████████    ██████████                                  ████
# ████                                                          ████████████████████    ████████████                                  ████
# ████                                                          ████████████████████    ████████████                                  ████
# ████                                                          ██████████████████    ████████████                                    ████
# ████                                                        ████████████████████  ██████████████                                    ████
# ████                                                        ██████████████████    ██████████████                                    ████
# ████                                                        ██████████████████  ████████████████                                    ████
# ████                                                        ████████████████      ██████████████                                    ████
# ████                                                        ████████████████      ██████████████                                    ████
# ████                                                        ██████████████        ████████████                                      ████
# ████                                                        ██████████████        ████████████                                      ████
# ████                                                      ██████████████          ██████████████                                    ████
# ████                                                      ██████████████        ████████████████                                    ████
# ████                                                    ██████████████████      ████████████████████                                ████
# ████                                                    ████████████████████      ████████████████████████                          ████
# ████                                                    ██████████████████████████      ██████████████████████                      ████
# ████                                                    ██████████████████████████████  ██████████████████████                      ████
# ████                                                    ██████████████████████████████                                              ████
# ████                                                                                                                                ████
# ████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████
# ████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████
