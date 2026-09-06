module Settings
  class << self
    def reset!
      @data = {
        # ------------ Audio ------------
        :master_volume          => 100,
        :bgm_volume             => 100,
        :sfx_volume             => 100,
        :fight_crime_track      => false,
        :old_reprise_song       => false,

        # ------------ Video ------------
        :fullscreen             => false,
        :resolution             => 0,
        :graphics_preset        => 4,
        :colorblind             => false,
        :frameskip              => true,
        :twm_shader             => true,
        :fog                    => true,
        :light                  => true,
        :water                  => true,
      # :light_layer_detect     => true,
        :scaling_mode           => 0,
        :vsync                  => 0,
        :max_firefly_count      => 30,
        :max_shrimp_count       => 0,
        :footprints             => true,
        :footsplashes           => true,
	
        # ------------- UI --------------
        :in_game_timer          => false,
        :language               => 0,
        :fasttravel_ui          => 0,
        :mainmenu_background    => 0,
        :icons_style            => false,

        # ---------- Gameplay -----------
        :movement               => 0,
        :skip_text              => false,
        :en_purple_messagebox   => true,
        :enforce_april_fools    => false,
        :true_memory_mode       => false,
        :oneshot_mode           => false,
        :twm_shader_footprint   => true,
        :disable_pizzles_for_fuckin_gayland_users_gayland_devs_fix_your_bullshit_already => false,
        # qmq ^
        :wallpaper_mode         => 0,

        # ---------- Advanced -----------
        :crashlog_privacy       => false,
        :streamer_privacy       => false,
        :invalid_param_checks   => false,
        :replace_all_niko       => false,

        # ---------- Сontrols -----------
        :gamepad_type           => -1,
        :gamepad_face_style     => 0,
        :gamepad_led            => true,
        :gamepad_deadzone       => 5,

        # ------------ Debug ------------
        :log_level	            => 3,
        :debug                  => false,
        :debug_character        => false,
        :debug_text_scene_title => true,
        :debug_text             => false,
        :debug_picture_names    => false,
        :debug_lightmap         => false,
        :shutdown_dbus_on_quit  => false,
        :profiler               => false,
      # :test_preset            => 1,
      }
      reset_controls!
    end

    def reset_controls!
      @data.merge!({
        :controls_walk_down  => [
                                  KeyBind.key(Input::key_from_name("Down")),
                                  KeyBind.caxis(Input::c_axis_from_name("LeftY"), KeyBind::Positive),
                                  KeyBind.cbutton(Input::c_button_from_name("DpDown"))
                                ],
        :controls_walk_left  => [
                                  KeyBind.key(Input::key_from_name("Left")),
                                  KeyBind.caxis(Input::c_axis_from_name("LeftX"), KeyBind::Negative),
                                  KeyBind.cbutton(Input::c_button_from_name("DpLeft"))
                                ],
        :controls_walk_right => [
                                  KeyBind.key(Input::key_from_name("Right")),
                                  KeyBind.caxis(Input::c_axis_from_name("LeftX"), KeyBind::Positive),
                                  KeyBind.cbutton(Input::c_button_from_name("DpRight"))
                                ],
        :controls_walk_up    => [
                                  KeyBind.key(Input::key_from_name("Up")),
                                  KeyBind.caxis(Input::c_axis_from_name("LeftY"), KeyBind::Negative),
                                  KeyBind.cbutton(Input::c_button_from_name("DpUp"))
                                ],
        :controls_run        => [
                                  KeyBind.key(Input::key_from_name("Left Shift")),
                                  KeyBind.caxis(Input::c_axis_from_name("righttrigger"), KeyBind::Positive),
                                  KeyBind.cbutton(Input::c_button_from_name("x"))
                                ],
        # -----------------------------------------------------------------------------------------------------------
        :controls_action     => [
                                  KeyBind.key(Input::key_from_name("Z")),
                                  KeyBind.key(Input::key_from_name("Space")),
                                  KeyBind.cbutton(Input::c_button_from_name("a")),
                                ],
        :controls_deactivate => [
                                  KeyBind.key(Input::key_from_name("Left Shift")),
                                  KeyBind.cbutton(Input::c_button_from_name("back")),
                                  KeyBind.caxis(Input::c_axis_from_name("lefttrigger"), KeyBind::Positive),
                                ],
        :controls_cancel     => [
                                  KeyBind.key(Input::key_from_name("X")),
                                  KeyBind.key(Input::key_from_name("Escape")),
                                  KeyBind.cbutton(Input::c_button_from_name("b")),
                                ],
        :controls_menu       => [
                                  KeyBind.key(Input::key_from_name("A")),
                                  KeyBind.key(Input::key_from_name("Return")),
                                  KeyBind.cbutton(Input::c_button_from_name("start")),
                                ],
        :controls_items      => [
                                  KeyBind.key(Input::key_from_name("S")),
                                  KeyBind.cbutton(Input::c_button_from_name("y")),
                                ],
        :controls_nav_left   => [
                                  KeyBind.key(Input::key_from_name("Q")),
                                  KeyBind.cbutton(Input::c_button_from_name("leftshoulder")),
                                ],
        :controls_nav_right  => [
                                  KeyBind.key(Input::key_from_name("W")),
                                  KeyBind.cbutton(Input::c_button_from_name("rightshoulder")),
                                ],
        # -----------------------------------------------------------------------------------------------------------
        :controls_debug      => [
                                  KeyBind.key(Input::key_from_name("Left Ctrl")),
                                  KeyBind.cbutton(Input::c_button_from_name("rightstick")),
                                ],
      })
    end
  end
end

class Window_Settings
  # if parameter is linked to data in class Settings, then default value in this structure is ignored
  DATA = {
    "Help" => [
      {
        :type => :base, 
        :name => " - It's Dangerous!",
        :icons => [[1, 0]]
      },
      {
        :type => :base,
        :name => " - Work In Progress",
        :icons => [[0, 1]]
      },
      {
        :type => :base,
        :name => " - Game Restart Required",
        :icons => [[3, 0]]
      },
      {
        :type => :base,
        :name => " - Location Reload Required",
        :icons => [[1, 1]]
      },
    ],
    "Audio" => [
      {
        :type => :slider,
        :name => "Master Volume",
        :parameter => :master_volume,
        :min => 0,
        :max => 100,
        :icons => [[2, 1]],
      },
      {
        :type => :slider,
        :name => "BGM Volume",
        :parameter => :bgm_volume,
        :min => 0,
        :max => 100,
        :icons => [[2, 1]],
      },
      {
        :type => :slider,
        :name => "SFX Volume",
        :parameter => :sfx_volume,
        :min => 0,
        :max => 100,
        :icons => [[2, 1]],
      },
      { :type => :sep, :name => "Game tracks"},
      {
        :type => :bool,
        :name => "ITS TIME FOR FIGHT CRIME",
        :parameter => :fight_crime_track,
        :icons => [[3, 1]],
      },
      {
        :type => :bool,
        :name => "Use Old Self Contained Universe(Reprise) version",
        :icons => [[3, 1]],
        :parameter => :old_reprise_song
      },
    ],
    "Video" => [
      { :type => :sep, :name => "Screen"},
      {
        :type => :enum,
        :name => "Resolution",
        :parameter => Graphics::RESOLUTION_OVERRIDDEN ? nil : :resolution,
        :values => Graphics::RESOLUTION_OVERRIDDEN ? ["Resolution is redefined via config"] : Graphics.resolutions_names_list,
        :icons => [[0, 2]],
      },
      {
        :type => :bool,
        :name => "Fullscreen",
        :parameter => :fullscreen,
        :icons => [[1, 2]],
      },
      {
        :type => :enum,
        :name => "Scaling mode",
        :parameter => :scaling_mode,
        :values => ["Nearest Neighbor", "Smooth(old)"],
        :icons => [[2, 2]],
      },
      {
        :type => :bool,
        :name => "Frameskip",
        :parameter => :frameskip,
        :icons => [[5, 3]],
      },
      {
        :type => :enum,
        :name => "Vsync mode",
        :parameter => :vsync,
        :values => ["Normal", "Adaptive", "Disabled"],
        :icons => [[2, 3]],
      },
      {
        :type => :presets,
        :name => "Graphics preset",
        :parameter => :graphics_preset,
        :custom_text => "Custom",
        :values => ["Potato", "Low", "Normal", "High"],
        :icons => [[3, 2]],
        :presets => [
          {
            :light => false,
            :water => false,
            :twm_shader => false,
            :fog => false,
            :max_firefly_count => 0,
            :footprints => false,
            :footsplashes => false,
          },
          {
            :light => false,
            :water => false,
            :twm_shader => false,
            :fog => true,
            :max_firefly_count => 15,
            :footprints => true,
            :footsplashes => true,
          },
          {
            :light => true,
            :water => false,
            :twm_shader => false,
            :fog => true,
            :max_firefly_count => 30,
            :footprints => true,
            :footsplashes => true,
          },
          {
            :light => true,
            :water => true,
            :twm_shader => true,
            :fog => true,
            :max_firefly_count => 30,
            :footprints => true,
            :footsplashes => true, 
          },
        ]
      },
      { :type => :sep, :name => "Effects"},
      {
        :type => :bool,
        :name => "Dynamic Light",
        :parameter => :light,
        :icons => [[3, 3]],
      },
      {
        :type => :bool,
        :name => "Dynamic Water",
        :parameter => :water,
        :icons => [[1, 3]],
      },
      {
        :type => :bool,
        :name => "Fog",
        :icons => [[4, 0]],
        :parameter => :fog
      },
      {
        :type => :bool,
        :name => "World Machine shader",
        :parameter => :twm_shader,
        :additional_icons => [[0, 1]],
        :icons => [[6, 0]],
      },
      { :type => :sep, :name => "Particles"},
      {
        :type => :slider,
        :name => "Density of glen fireflies",
        :parameter => :max_firefly_count,
        :additional_icons => [[1, 1]],
        :icons => [[4, 1]],
        :min => 0,
        :max => 30
      },
      {
        :type => :slider,
        :name => "Density of shrims (!EXPEREMENTAL!)",
        :parameter => :max_shrimp_count,
        :additional_icons => [[0, 1], [1, 1]],
        :icons => [[4, 2]],
        :min => 0,
        :max => 80
      },
      {
        :type => :bool,
        :name => "Footprints",
        :parameter => :footprints,
        :icons => [[4, 3]],
      },
      {
        :type => :bool,
        :name => "Footsplashes",
        :parameter => :footsplashes,
        :icons => [[5, 0]],
      },
    ],
    "UI" => [
      {
        :type => :bool,
        :name => "In-Game Timer",
        :icons => [[5, 1]],
        :parameter => :in_game_timer
      },
      {
        :type => :enum,
        :name => "Language",
        :parameter => :language,
        :icons => [[5, 2]],
        :values => Language::LANGUAGES
      },
      {
        :type => :enum,
        :name => "FastTravel UI",
        :parameter => :fasttravel_ui,
        :icons => [[6, 3]],
        :additional_icons => [[0, 1]],
        :values => ["Original", "WME (WIP)"]
      },
      {
        :type => :enum,
        :name => "Main Menu background",
        :parameter => :mainmenu_background,
        :icons => [[6, 1]],
        :values => ["Original", "Badend", "Title Black"]
      },
      {
        :type => :custom,
        :name => "Icons Style",
        :parameter => :icons_style,
        :icons => [[5, 4]],
        :callbacks => {
          :value_set => proc { |super_proc, value|
            if (value != self.value)
              Audio.se_play(PARAMETER_CHANGE_AUDIO, 70, value ? 125 : 75)
            end
            super_proc.call(value)
          },
          :get_display_value => proc { |super_proc|
            self.value ? "Mono" : "Colored"
          },
          :value_left => proc { |super_proc|
            return if @disabled
            self.value = !self.value
            @settings_content.icons_atlas = self.value ? RPG::Cache.menu("icons_mono") : RPG::Cache.menu("icons")
            @settings_content.redraw_all
          },
          :value_right => proc { |super_proc|
            return if @disabled
            self.value = !self.value
            @settings_content.icons_atlas = self.value ? RPG::Cache.menu("icons_mono") : RPG::Cache.menu("icons")
            @settings_content.redraw_all
          },
          :action => proc { |super_proc|
            return if @disabled
            self.value = !self.value
            @settings_content.icons_atlas = self.value ? RPG::Cache.menu("icons_mono") : RPG::Cache.menu("icons")
            @settings_content.redraw_all
          }
        }
      }
    ],
    "Gameplay" => [
      { :type => :sep, :name => "Vanilla" },
      {
        :type => :bool,
        :name => "Colorblind mode",
        :icons => [[6, 2]],
        :parameter => :colorblind
      },
      {
        :type => :enum,
        :name => "Default movement",
        :parameter => :movement,
        :icons => [[6, 3]],
        :values => ["Walk", "Run"]
      },
      {
        :type => :bool,
        :name => "Skip Text (R)",
        :icons => [[2, 4]],
        :parameter => :skip_text
      },
      { :type => :sep, :name => "Sunshine" },
      {
      	:type => :bool,
        :name => "Purple Message Box for Entity",
        :icons => [[6, 0]],
        :parameter => :en_purple_messagebox
      },
      {
        :type => :bool,
        :name => "World Machine shader on Entity footprints",
        :icons => [[6, 0]],
        :additional_icons => [[0, 1]],
        :parameter => :twm_shader_footprint
      },
      {
        :type => :bool,
        :name => "True Memory Mode",
        :icons => [[6, 0]],
        :parameter => :true_memory_mode
      },
      {
        :type => :enum,
        :name => "Wallpaper Mode",
        :icons => [[7, 2]],
        :parameter => :wallpaper_mode,
        :values => ["Normal", "Fallback", "Disabled"]
      },
      {
        :type => :bool,
        :name => "Freeware Mode",
        :icons => [[3, 4]],
        :additional_icons => [[1, 0]],
        :parameter => :oneshot_mode
      },
      {
        :type => :bool,
        :name => "Enforce April Fools",
        :icons => [[7, 0]],
        :parameter => :enforce_april_fools
      },
      {
        :type => :bool,
        :name => "Disable System API Dependent Puzzles (For wayland users)",
        :icons => [[7, 1]],
        :parameter => :disable_pizzles_for_fuckin_gayland_users_gayland_devs_fix_your_bullshit_already
      },
    ],
    "Controls" => [
      { :type => :sep, :name => "Gamepad" },
      {
        :type => :custom,
        :name => "Your Gamepad type",
        :parameter => :gamepad_type,
        :icons => [[1, 4]],
        :callbacks => {
          :init => proc { |super_proc|
            @max_value = GamepadIcons::GAMEPADS.length
            @texts = [
              "Auto",
              "XBOX 360",
              "XBOX One",
              "XBOX Series X",
              "PS 3",
              "PS 4",
              "PS 5",
              "Nintendo Switch Pro",
              "Nintendo Joycons",
              "Amazon Luna",
              "OUYA",
              "Google Stadia",
              "Steam Controller",
              "Steam Deck",
              "Nintendo GAMECUBE"
            ]

            super_proc.call
          },
          :get_display_value => proc { |super_proc|
            ""
          },
          :redraw => proc { |super_proc|
            @sprite.bitmap.clear
            redraw_icons
            redraw_separator
            redraw_title

            x, y = GamepadIcons.icon()

            @sprite.bitmap.stretch_blt(Rect.new(PARAMETER_WIDTH - ICON_SIZE * 2, PARAMETER_HEIGHT / 2 - ICON_SIZE, ICON_SIZE * 2, ICON_SIZE * 2), RPG::Cache.menu("gamepad_icons"), Rect.new(x * ICON_SIZE, y * ICON_SIZE, ICON_SIZE, ICON_SIZE))
            @sprite.bitmap.draw_text(@sprite.bitmap.width - @value_width - ICON_SIZE * 2 - 8, 0, @value_width, @sprite.bitmap.height, tr(@texts[self.value]), 2)
          },
          :value_set => proc { |super_proc, value|
            if (value != self.value)
              Audio.se_play(PARAMETER_CHANGE_AUDIO, 70, (value.to_f / @max_value.to_f * 50.0).to_i + 75)
            end
            super_proc.call(value)
          },
          :value_left => proc { |super_proc|
            return if @disabled
            self.value = (self.value - 1) % @max_value
            @settings_content.redraw_all
          },
          :value_right => proc { |super_proc|
            return if @disabled
            self.value = (self.value + 1) % @max_value
            @settings_content.redraw_all
          }
        }
      },
      {
        :type => :custom,
        :name => "Face Buttons style",
        :icons => [[1, 4]],
        :parameter => :gamepad_face_style,
        :callbacks => {
          :init => proc { |super_proc|
            @max_value = 5

            super_proc.call
          },
          :get_display_value => proc { |super_proc|
            "" # meow >w<
          },
          :redraw => proc { |super_proc|
            @disabled = !GamepadIcons.face_skinnable

            @sprite.bitmap.clear
            redraw_icons
            redraw_separator
            redraw_title

            sx, sy = GamepadIcons.button(Input::GamepadButton::SOUTH, false)
            wx, wy = GamepadIcons.button(Input::GamepadButton::WEST, false)
            nx, ny = GamepadIcons.button(Input::GamepadButton::NORTH, false)
            ex, ey = GamepadIcons.button(Input::GamepadButton::EAST, false)
            if GamepadIcons.face_skinnable
              sy += self.value
              wy += self.value
              ny += self.value
              ey += self.value
            end

            @sprite.bitmap.stretch_blt(Rect.new(PARAMETER_WIDTH - ICON_SIZE * 8, PARAMETER_HEIGHT / 2 - ICON_SIZE, ICON_SIZE * 2, ICON_SIZE * 2), RPG::Cache.menu("gamepad_icons"), Rect.new(sx * ICON_SIZE, sy * ICON_SIZE, ICON_SIZE, ICON_SIZE))
            @sprite.bitmap.stretch_blt(Rect.new(PARAMETER_WIDTH - ICON_SIZE * 6, PARAMETER_HEIGHT / 2 - ICON_SIZE, ICON_SIZE * 2, ICON_SIZE * 2), RPG::Cache.menu("gamepad_icons"), Rect.new(wx * ICON_SIZE, wy * ICON_SIZE, ICON_SIZE, ICON_SIZE))
            @sprite.bitmap.stretch_blt(Rect.new(PARAMETER_WIDTH - ICON_SIZE * 4, PARAMETER_HEIGHT / 2 - ICON_SIZE, ICON_SIZE * 2, ICON_SIZE * 2), RPG::Cache.menu("gamepad_icons"), Rect.new(nx * ICON_SIZE, ny * ICON_SIZE, ICON_SIZE, ICON_SIZE))
            @sprite.bitmap.stretch_blt(Rect.new(PARAMETER_WIDTH - ICON_SIZE * 2, PARAMETER_HEIGHT / 2 - ICON_SIZE, ICON_SIZE * 2, ICON_SIZE * 2), RPG::Cache.menu("gamepad_icons"), Rect.new(ex * ICON_SIZE, ey * ICON_SIZE, ICON_SIZE, ICON_SIZE))
          },
          :value_set => proc { |super_proc, value|
            if (value != self.value)
              Audio.se_play(PARAMETER_CHANGE_AUDIO, 70, (value.to_f / @max_value.to_f * 50.0).to_i + 75)
            end
            super_proc.call(value)
          },
          :value_left => proc { |super_proc|
            next if @disabled
            self.value = (self.value - 1) % @max_value
            @settings_content.redraw_all
          },
          :value_right => proc { |super_proc|
            next if @disabled
            self.value = (self.value + 1) % @max_value
            @settings_content.redraw_all
          }
        }
      },
      {
        :type => :bool,
        :name => "Control LED lighting",
        :icons => [[6, 2]],
        :parameter => :gamepad_led
      },
      { :type => :sep, :name => "Walk" },
      {
        :type => :key,
        :name => "Walk Down",
        :parameter => :controls_walk_down,
        :bind => Input::DOWN
      },
      {
        :type => :key,
        :name => "Walk Left",
        :parameter => :controls_walk_left,
        :bind => Input::LEFT
      },
      {
        :type => :key,
        :name => "Walk Right",
        :parameter => :controls_walk_right,
        :bind => Input::RIGHT
      },
      {
        :type => :key,
        :name => "Walk Up",
        :parameter => :controls_walk_up,
        :bind => Input::UP
      },
      {
        :type => :key,
        :name => "Run",
        :parameter => :controls_run,
        :bind => Input::RUN
      },
      { :type => :sep, :name => "Actions" },
      {
        :type => :key,
        :name => "Action",
        :parameter => :controls_action,
        :bind => Input::ACTION
      },
      {
        :type => :key,
        :name => "Deactivate",
        :parameter => :controls_deactivate,
        :bind => Input::DEACTIVATE
      },
      {
        :type => :key,
        :name => "Cancel",
        :parameter => :controls_cancel,
        :bind => Input::CANCEL
      },
      {
        :type => :key,
        :name => "Menu",
        :parameter => :controls_menu,
        :bind => Input::MENU
      },
      {
        :type => :key,
        :name => "Items",
        :parameter => :controls_items,
        :bind => Input::ITEMS
      },
      {
        :type => :key,
        :name => "Nav Left",
        :parameter => :controls_nav_left,
        :bind => Input::L
      },
      {
        :type => :key,
        :name => "Nav Right",
        :parameter => :controls_nav_right,
        :bind => Input::R
      },
      {
        :type => :action,
        :name => "Reset Controls",
        :icons => [[1, 0]],
        :action => Settings.method(:reset_controls!)
      },
    ],
    "Advanced" => [
      { :type => :sep, :name => "Privacy" },
      {
        :type => :bool,
        :name => "Crashlog privacy",
        :icons => [[7, 3]],
        :parameter => :crashlog_privacy
      },
      {
        :type => :bool,
        :name => "Streamer privacy",
        :icons => [[7, 3]],
        :parameter => :streamer_privacy
      },
      { :type => :sep, :name => "Other" },
      {
        :type => :bool,
        :name => "SDL_HINT_INVALID_PARAM_CHECKS",
        :icons => [[1, 4]],
        :additional_icons => [[1, 0]],
        :parameter => :invalid_param_checks
      },
      {
        :type => :bool,
        :name => "Oneshot Community Mode",
        :icons => [[7, 0]],
        :additional_icons => [[1, 0]],
        :parameter => :replace_all_niko
      },
    ],
    "Debug" => [
      {
        :type => :enum,
        :name => "Log Level",
        :parameter => :log_level,
        :values => ["Fatal Error", "Error", "Warning", "Info", "Debug"],
        :icons => [[1, 0]],
      },
      {
        :type => :bool,
        :name => "Debug mode",
        :icons => [[1, 0]],
        :parameter => :debug
      },
      {
        :type => :bool,
        :name => "Show debug character",
        :parameter => :debug_character,
        :icons => [[1, 0]],
      },
      {
        :type => :bool,
        :name => "Draw debug text in main menu",
        :parameter => :debug_text_scene_title,
        :icons => [[1, 0]],
      },
      {
        :type => :bool,
        :name => "Show debug text",
        :parameter => :debug_text,
        :icons => [[1, 0]],
      },
      {
        :type => :bool,
        :name => "Show picture names",
        :parameter => :debug_picture_names,
        :icons => [[1, 0]],
      },
      {
        :type => :bool,
        :name => "Debug lightmap",
        :parameter => :debug_lightmap,
        :icons => [[1, 0]],
      },
      {
        :type => :bool,
        :name => "SDL_HINT_SHUTDOWN_DBUS_ON_QUIT",
        :parameter => :shutdown_dbus_on_quit,
        :icons => [[1, 0]],
      },
      {
        :type => :bool,
        :name => "Profiler",
        :parameter => :profiler,
        :icons => [[1, 0]],
      },
      :sep,
      {
        :type => :key,
        :name => "Debug",
        :parameter => :controls_debug,
        :bind => Input::DEBUGACTION,
        :icons => [[1, 0]],
      },
      :sep,
      {
        :type => :action,
        :name => "Clear image cache",
        :action => Proc.new { RPG::Cache.clear },
        :icons => [[1, 0]],
      },
      {
        :type => :action,
        :name => "Do Crash",
        :action => Proc.new { raise "Test" },
        :icons => [[1, 0]],
      },
    ],
    "Mods" => [
      {
        :type => :base,
        :name => "Modloader enabled?",
        :init_value => ModLoader::IS_ENABLED ? "ON" : "OFF",
        :icons => [[1, 4]],
        :disabled => true
      },
    ],
  }

  # Part of ModAPI
  def self.add_setting(setting)
    DATA["Mods"] << setting
  end
end

