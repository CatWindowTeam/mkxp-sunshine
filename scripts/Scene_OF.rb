class Scene_OF
  BPM = 135
  BPM_TIME = 1.0 / (BPM / 60.0) * Graphics.frame_rate * 2

  BUMP_SCALE = 1.07
  BUMP_RETURN_SPEED = 0.1

  ARROW_SIZE = 32
  ARROW_SCALE = 2
  ARROWS_PADDING = 12
  ARROWS_TOP_MARGIN = 32
  ARROWS_SIDES_MARGIN = 48

  ARROWS_SPEED = 10
  ARROWS_ALLOW_ZONE_SIZE = 40

  HP_WIDTH = 400
  HP_HEIGHT = 16

  MISS_TIME = 90

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
    @viewport_arrows = Viewport.new(0, 0, Graphics.width, Graphics.height)
    @viewport_ui.ox = @viewport_arrows.ox = -Graphics.width / 2
    @viewport_ui.oy = @viewport_arrows.oy = -Graphics.height / 2
    @spritesheet = RPG::Cache.misc(".cats")

    @cat_flying_arrows = []
    @player_flying_arrows = []
    @player_arrows = []
    @cat_arrows = []
    for i in 0..3
      player_arrow = Sprite.new(@viewport_ui)
      player_arrow.bitmap = Bitmap.new(ARROW_SIZE, ARROW_SIZE)
      player_arrow.bitmap.stretch_blt(Rect.new(0, 0, ARROW_SIZE, ARROW_SIZE), @spritesheet, SPRITES[PLAYER_ARROWS_SPRITES[:base][i]])
      player_arrow.zoom_x = player_arrow.zoom_y = ARROW_SCALE
      player_arrow.x = Graphics.width / 2 - ARROWS_SIDES_MARGIN + (ARROW_SIZE * ARROW_SCALE + ARROWS_PADDING) * i - (ARROW_SIZE * ARROW_SCALE * 4 + ARROWS_PADDING * 3)
      player_arrow.y = ARROWS_TOP_MARGIN - Graphics.height / 2
      @player_arrows << player_arrow

      cat_arrow = Sprite.new(@viewport_ui)
      cat_arrow.bitmap = Bitmap.new(ARROW_SIZE, ARROW_SIZE)
      cat_arrow.bitmap.stretch_blt(Rect.new(0, 0, ARROW_SIZE, ARROW_SIZE), @spritesheet, SPRITES[CAT_ARROWS_SPRITES[:base][i]])
      cat_arrow.zoom_x = cat_arrow.zoom_y = ARROW_SCALE
      cat_arrow.x = ARROWS_SIDES_MARGIN + (ARROW_SIZE * ARROW_SCALE + ARROWS_PADDING) * i - Graphics.width / 2
      cat_arrow.y = ARROWS_TOP_MARGIN - Graphics.height / 2
      @cat_arrows << cat_arrow
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

    @hp = 50

    @bst_debug = Sprite.new(@viewport_ui)
    @bst_debug.bitmap = Bitmap.new(300, 40)

    @total_time = 0
    @bump_timeout = 0
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

    # fl studio b:s:t :3
    beat = (@total_time / BPM_TIME / 2).to_i + 1
    step = (@total_time / BPM_TIME * 15 / 2).to_i % 15 + 1
    tick = (@total_time / BPM_TIME * 24 * 15 / 2).to_i % 24
    
    @bst_debug.bitmap.clear
    @bst_debug.bitmap.draw_text(Rect.new(0, 0, 300, 40), "#{beat} : #{step} : #{tick}")

    @cat_flying_arrows.each do |arrow|
      arrow[0].y -= ARROWS_SPEED
      if arrow[0].y < ARROWS_TOP_MARGIN - Graphics.height / 2
        arrow[0].dispose
        @cat_flying_arrows.delete(arrow)
      end
    end
    @player_flying_arrows.each do |arrow|
      arrow[0].y -= ARROWS_SPEED
      if arrow[0].y < - Graphics.height / 2 - ARROW_SIZE * ARROW_SCALE
        miss
        arrow[0].dispose
        @player_flying_arrows.delete(arrow)
      end
    end

    @bump_timeout -= 1
    if @bump_timeout <= 0
      @bump_timeout += BPM_TIME
      @viewport_arrows.scale_x = @viewport_ui.scale_x = BUMP_SCALE
      @viewport_arrows.scale_y = @viewport_ui.scale_y = BUMP_SCALE
    end
    @viewport_arrows.scale_x = @viewport_ui.scale_x = @viewport_ui.scale_x * (1.0 - BUMP_RETURN_SPEED) + BUMP_RETURN_SPEED
    @viewport_arrows.scale_y = @viewport_ui.scale_y = @viewport_ui.scale_y * (1.0 - BUMP_RETURN_SPEED) + BUMP_RETURN_SPEED

    # player
    for i in 1..4
      mapped = map_input(i * 2)
      is_pressed = Input.press?(i * 2)
      if @arrows_states[mapped] != is_pressed
        @arrows_states[mapped] = is_pressed
        @player_arrows[mapped].bitmap.clear
        @player_arrows[mapped].bitmap.stretch_blt(Rect.new(0, 0, ARROW_SIZE, ARROW_SIZE), @spritesheet, SPRITES[PLAYER_ARROWS_SPRITES[@arrows_states[mapped] ? :pressed : :base][mapped]])
      end
      
      if Input.trigger?(i * 2)
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
                @miss_sound.stop
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

  def miss
    self.hp -= 10
    @miss_sound.play(0, 0.6, rand(75..125) / 100.0)
    @miss_timeout = MISS_TIME
  end

  def hp
    @hp
  end
  def hp=(val)
    @hp = val.clamp(0, 100)
    @hp_white.zoom_x = (HP_WIDTH - 4) * (100 - @hp) / 100.0
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
    SONG_MAPPING.each do |line_data|
      beat = line_data[1]
      step = line_data[2]
      tick = line_data[3]
      for i in 0..3
        if line_data[0][i] == '#'
          arrow = make_arrow(i, true)
          arrow.x = ARROWS_SIDES_MARGIN + (ARROW_SIZE * ARROW_SCALE + ARROWS_PADDING) * i - Graphics.width / 2
          arrow.y = ARROWS_TOP_MARGIN - Graphics.height / 2 + 
                    (beat - 1) * 2 * BPM_TIME * ARROWS_SPEED + 
                    (step - 1) / 15.0 * 2 * BPM_TIME * ARROWS_SPEED + 
                    (tick) / 24.0 / 15.0 * 2 * BPM_TIME * ARROWS_SPEED

          @cat_flying_arrows << [arrow, i]
        end

        if line_data[0][i + 5] == '#'
          arrow = make_arrow(i, false)
          arrow.x = Graphics.width / 2 - ARROWS_SIDES_MARGIN + (ARROW_SIZE * ARROW_SCALE + ARROWS_PADDING) * i - (ARROW_SIZE * ARROW_SCALE * 4 + ARROWS_PADDING * 3)
          arrow.y = ARROWS_TOP_MARGIN - Graphics.height / 2 + 
                    (beat - 1) * 2 * BPM_TIME * ARROWS_SPEED + 
                    (step - 1) / 15.0 * 2 * BPM_TIME * ARROWS_SPEED + 
                    (tick) / 24.0 / 15.0 * 2 * BPM_TIME * ARROWS_SPEED

          @player_flying_arrows << [arrow, i]
        end
      end
    end
  end

  def make_arrow(direction, meow)
    arrow = Sprite.new(@viewport_arrows)
    arrow.bitmap = Bitmap.new(ARROW_SIZE, ARROW_SIZE)
    arrow.bitmap.stretch_blt(Rect.new(0, 0, ARROW_SIZE, ARROW_SIZE), @spritesheet, SPRITES[(meow ? CAT_ARROWS_SPRITES : PLAYER_ARROWS_SPRITES)[:target][direction]])
    arrow.zoom_x = arrow.zoom_y = ARROW_SCALE
    arrow
  end

  SONG_MAPPING = [
    # arrows,      B,  S,  T
    ["#   |    ",  5,  1,  0],
    ["  # |    ",  5,  3,  0],
    [" #  |    ",  5,  5,  0],
    ["   #|    ",  5,  9,  0],
    ["  # |    ",  5, 13,  0],
    ["#   |    ",  6,  1,  0],
    [" #  |    ",  6,  3,  0],
    ["  # |    ",  6,  5,  0],
    ["  # |    ",  6,  9,  0],
    ["#   |    ",  6, 13,  0],
    [" #  |    ",  7,  1,  0],
    ["   #|    ",  7,  3,  0],
    ["  # |    ",  7,  5,  0],
    ["#   |    ",  7,  9,  0],
    ["#   |    ",  7, 13,  0],
    ["#   |    ",  8,  1,  0],
    [" #  |    ",  8,  3,  0],
    [" #  |    ",  8,  6,  0],
    ["  # |    ",  8,  8,  0],
    [" #  |    ",  8, 11,  0],
    ["   #|    ",  8, 13,  0],

    ["    |#   ",  9,  1,  0],
    ["    |  # ",  9,  3,  0],
    ["    | #  ",  9,  5,  0],
    ["    |   #",  9,  9,  0],
    ["    |  # ",  9, 13,  0],
    ["    |#   ", 10,  1,  0],
    ["    | #  ", 10,  3,  0],
    ["    |  # ", 10,  5,  0],
    ["    |  # ", 10,  9,  0],
    ["    |#   ", 10, 13,  0],
    ["    | #  ", 11,  1,  0],
    ["    |   #", 11,  3,  0],
    ["    |  # ", 11,  5,  0],
    ["    |#   ", 11,  9,  0],
    ["    |#   ", 11, 13,  0],
    ["    |#   ", 12,  1,  0],
    ["    | #  ", 12,  3,  0],
    ["    | #  ", 12,  5,  0],
    ["    | #  ", 12,  6,  0],
    ["    | #  ", 12,  7,  0],
    ["    |  # ", 12,  8,  0],
    ["    | #  ", 12,  9,  0],
    ["    |   #", 12, 10,  0],
    ["    | #  ", 12, 11,  0],
    ["    |#   ", 12, 13,  0],
    ["    |#   ", 12, 15,  0],
   #["    |#   ", 12, 15, 12],
    ["    |#   ", 12, 16,  0],
   #["    |#   ", 12, 16, 12],
    ["    |#   ", 13,  1,  0],
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
