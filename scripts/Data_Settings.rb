module Settings
  class << self
    def reset!
      @data = {
        # Audio
        :bgm_volume                  => 100,
        :sfx_volume                  => 100,

        # Video
        :fullscreen                  => false,
        :resolution                  => 0,
        :colorblind                  => false,
        :frameskip                   => true,
        :twm_shader                  => true,
        :light                       => true,
        :light_shadows               => true,

        # UI
        :in_game_timer               => false,
        :language                    => 0,
        :fasttravel_ui               => 0,

        # Gameplay
        :movement                    => 0,
        :skip_text                   => false,
        :en_purple_messagebox        => true,
        :enforce_april_fools         => false,
        :true_memory_mode            => false,
        :dejavu_mode                 => false,
        :oneshot_mode                => false,

        # Advanced

        #controls
        :gamepad_led                 => true,
        :gamepad_deadzone            => 5,

        # Debug
        :debug                       => false,
        :debug_character             => false,
        :debug_text                  => false,
        :debug_text_scene_title      => true,
        :debug_picture_names         => false,
        :debug_lightmap              => false,
      }
      reset_controls!
    end

    def reset_controls!
      @data.merge!({
        :controls_walk_down                   => [
                                          KeyBind.key(Input::key_from_name("Down")),
                                          KeyBind.caxis(Input::c_axis_from_name("LeftY"), KeyBind::Positive),
                                          KeyBind.cbutton(Input::c_button_from_name("DpDown"))
                                        ],
        :controls_walk_left                   => [
                                          KeyBind.key(Input::key_from_name("Left")),
                                          KeyBind.caxis(Input::c_axis_from_name("LeftX"), KeyBind::Negative),
                                          KeyBind.cbutton(Input::c_button_from_name("DpLeft"))
                                        ],
        :controls_walk_right                  => [
                                          KeyBind.key(Input::key_from_name("Right")),
                                          KeyBind.caxis(Input::c_axis_from_name("LeftX"), KeyBind::Positive),
                                          KeyBind.cbutton(Input::c_button_from_name("DpRight"))
                                        ],
        :controls_walk_up                     => [
                                          KeyBind.key(Input::key_from_name("Up")),
                                          KeyBind.caxis(Input::c_axis_from_name("LeftY"), KeyBind::Negative),
                                          KeyBind.cbutton(Input::c_button_from_name("DpUp"))
                                        ],
        :controls_run                    => [
                                          KeyBind.key(Input::key_from_name("Left Shift")),
                                          KeyBind.caxis(Input::c_axis_from_name("righttrigger"), KeyBind::Positive),
                                          KeyBind.cbutton(Input::c_button_from_name("x"))
                                        ],
        # -----------------------------------------------------------------------------------------------------------
        :controls_action                => [
                                          KeyBind.key(Input::key_from_name("Z")),
                                          KeyBind.key(Input::key_from_name("Space")),
                                          KeyBind.cbutton(Input::c_button_from_name("a")),
                                        ],
        :controls_deactivate            => [
                                          KeyBind.key(Input::key_from_name("Left Shift")),
                                          KeyBind.cbutton(Input::c_button_from_name("back")),
                                          KeyBind.caxis(Input::c_axis_from_name("lefttrigger"), KeyBind::Positive),
                                        ],
        :controls_cancel                => [
                                          KeyBind.key(Input::key_from_name("X")),
                                          KeyBind.key(Input::key_from_name("Escape")),
                                          KeyBind.cbutton(Input::c_button_from_name("b")),
                                        ],
        :controls_menu                  => [
                                          KeyBind.key(Input::key_from_name("A")),
                                          KeyBind.key(Input::key_from_name("Return")),
                                          KeyBind.cbutton(Input::c_button_from_name("start")),
                                        ],
        :controls_items                 => [
                                          KeyBind.key(Input::key_from_name("S")),
                                          KeyBind.cbutton(Input::c_button_from_name("y")),
                                        ],
        :controls_nav_left              => [
                                          KeyBind.key(Input::key_from_name("Q")),
                                          KeyBind.cbutton(Input::c_button_from_name("leftshoulder")),
                                        ],
        :controls_nav_right             => [
                                          KeyBind.key(Input::key_from_name("W")),
                                          KeyBind.cbutton(Input::c_button_from_name("rightshoulder")),
                                        ],
        # -----------------------------------------------------------------------------------------------------------
        :controls_debug                 => [
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
    tr("Audio") => [
      {
        :type => :slider,
        :name => tr('BGM Volume'),
        :parameter => :bgm_volume,
        :min => 0,
        :max => 100
      },
      {
        :type => :slider,
        :name => tr('SFX Volume'),
        :parameter => :sfx_volume,
        :min => 0,
        :max => 100
      },
    ],
    tr("Video") => [
      {
        :type => :enum,
        :name => tr('Resolution'),
        :default => 0,
        :parameter => Graphics::RESOLUTION_OVERRIDDEN ? nil : :resolution,
        :values => Graphics::RESOLUTION_OVERRIDDEN ? ["Resolution is redefined via config"] : Graphics.resolutions_names_list
      },
      {
        :type => :bool,
        :name => tr('Fullscreen'),
        :parameter => :fullscreen
      },
      {
        :type => :bool,
        :name => tr('Colorblind mode'),
        :parameter => :colorblind
      },
      {
        :type => :bool,
        :name => tr('Frameskip'),
        :parameter => :frameskip
      },
      {
        :type => :bool,
        :name => tr('World machine shader'),
        :parameter => :twm_shader
      },
      { :type => :sep, :name => tr("Lighting")},
      {
        :type => :bool,
        :name => tr('Dynamic Light'),
        :parameter => :light,
      },
      #{
      #  :type => :bool,
      #  :name => tr('Shadows'),
      #  :parameter => :light_shadows
      #},
    ],
    tr("UI") => [
      {
        :type => :bool,
        :name => tr('In-Game Timer'),
        :parameter => :in_game_timer
      },
      {
        :type => :enum,
        :name => tr('Language'),
        :parameter => :language,
        :values => Language::LANGUAGES
      },
      {
        :type => :enum,
        :name => tr('FastTravel UI'),
        :parameter => :fasttravel_ui,
        :values => [tr("Original"), tr("WME (WIP)")]
      },
    ],
    tr("Gameplay") => [
      {
        :type => :enum,
        :name => tr('Default movement'),
        :parameter => :movement,
        :values => [tr("Walk"), tr("Run")]
      },
      {
        :type => :bool,
        :name => tr('Skip Text (R)'),
        :parameter => :skip_text
      },
      {
        :type => :bool,
        :name => tr('Purple message box for Entity(Not implemented)'),
        :parameter => :en_purple_messagebox
      },
      {
        :type => :bool,
        :name => tr('Enforce april fools'),
        :parameter => :enforce_april_fools
      },
      {
        :type => :bool,
        :name => tr('True Memory Mode(Not implemented)'),
        :parameter => :true_memory_mode
      },
      {
        :type => :switch,
        :name => tr("Dejavu mode"),
        :switch => 399,
        :parameter => :dejavu_mode,
        :default => false,
        :invert => false,
      },
      {
        :type => :bool,
        :name => tr('Freeware Mode(!)'),
        :parameter => :oneshot_mode
      },
    ],
    tr("Controls") => [
      {
        :type => :bool,
        :name => tr('Control LED lighting on gamepads'),
        :parameter => :gamepad_led
      },
      { :type => :sep, :name => tr("Walk") },
      {
        :type => :key,
        :name => tr("Walk Down"),
        :parameter => :controls_walk_down,
        :bind => Input::DOWN
      },
      {
        :type => :key,
        :name => tr("Walk Left"),
        :parameter => :controls_walk_left,
        :bind => Input::LEFT
      },
      {
        :type => :key,
        :name => tr("Walk Right"),
        :parameter => :controls_walk_right,
        :bind => Input::RIGHT
      },
      {
        :type => :key,
        :name => tr("Walk Up"),
        :parameter => :controls_walk_up,
        :bind => Input::UP
      },
      {
        :type => :key,
        :name => tr("Run"),
        :parameter => :controls_run,
        :bind => Input::RUN
      },
      { :type => :sep, :name => tr("Actions") },
      {
        :type => :key,
        :name => tr("Action"),
        :parameter => :controls_action,
        :bind => Input::ACTION
      },
      {
        :type => :key,
        :name => tr("Deactivate"),
        :parameter => :controls_deactivate,
        :bind => Input::DEACTIVATE
      },
      {
        :type => :key,
        :name => tr("Cancel"),
        :parameter => :controls_cancel,
        :bind => Input::CANCEL
      },
      {
        :type => :key,
        :name => tr("Menu"),
        :parameter => :controls_menu,
        :bind => Input::MENU
      },
      {
        :type => :key,
        :name => tr("Items"),
        :parameter => :controls_items,
        :bind => Input::ITEMS
      },
      {
        :type => :key,
        :name => tr("Nav Left"),
        :parameter => :controls_nav_left,
        :bind => Input::L
      },
      {
        :type => :key,
        :name => tr("Nav Right"),
        :parameter => :controls_nav_right,
        :bind => Input::R
      },
      #{ :type => :sep, :name => tr("Other") },
      {
        :type => :action,
        :name => tr("Reset Controls"),
        :action => Settings.method(:reset_controls!)
      },
    ],
    tr("Advanced") => [
      {
        :type => :bool,
        :name => tr('useless option:3'),
        :default => false
      },
    ],
    tr("Debug") => [
      {
        :type => :bool,
        :name => tr('Debug mode(!)'),
        :parameter => :debug
      },
      {
        :type => :bool,
        :name => tr('Show debug character'),
        :parameter => :debug_character
      },
      {
        :type => :bool,
        :name => tr('Draw debug text in main menu'),
        :parameter => :debug_text_scene_title
      },
      {
        :type => :bool,
        :name => tr('Show debug text'),
        :parameter => :debug_text
      },
      {
        :type => :bool,
        :name => tr('Show picture names'),
        :parameter => :debug_picture_names
      },
      {
        :type => :bool,
        :name => tr('Debug lightmap'),
        :parameter => :debug_lightmap
      },
      { :type => :sep },
      {
        :type => :key,
        :name => tr("Debug"),
        :parameter => :controls_debug,
        :bind => Input::DEBUGACTION
      },
      { :type => :sep },
      {
        :type => :action,
        :name => tr("Clear image cache"),
        :action => Proc.new { RPG::Cache.clear }
      }
    ],
  }
end
