module Settings
  class << self
    def reset!
      @data = {
        # Audio
        :bgm_volume                  => 100,
        :sfx_volume                  => 100,
        :use_fight_crime_track       => false,

        # Video
        :fullscreen                  => false,
        :resolution                  => 0,
        :colorblind                  => false,
        :frameskip                   => true,
        :twm_shader                  => true,
        :light                       => true,
        :scaling_mode                => 0,

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
        :oneshot_mode                => false,
		:twm_shader_footprint        => true,

        # Advanced
		:crashlog_privacy        	 => false,
	
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


        #Hidden
        :is_dejavu                   => false,
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
    "Audio" => [
      {
        :type => :slider,
        :name => "BGM Volume",
        :parameter => :bgm_volume,
        :min => 0,
        :max => 100
      },
      {
        :type => :slider,
        :name => "SFX Volume",
        :parameter => :sfx_volume,
        :min => 0,
        :max => 100
      },
      {
        :type => :bool,
        :name => "ITS TIME FOR FIGHT CRIME",
        :parameter => :use_fight_crime_track
      },
    ],
    "Video" => [
      { :type => :sep, :name => "Screen"},
      {
        :type => :enum,
        :name => "Resolution",
        :default => 0,
        :parameter => Graphics::RESOLUTION_OVERRIDDEN ? nil : :resolution,
        :values => Graphics::RESOLUTION_OVERRIDDEN ? ["Resolution is redefined via config"] : Graphics.resolutions_names_list
      },
      {
        :type => :bool,
        :name => "Fullscreen",
        :parameter => :fullscreen
      },
      {
        :type => :bool,
        :name => "Frameskip",
        :parameter => :frameskip
      },
      {
        :type => :enum,
        :name => "Scaling mode",
        :parameter => :scaling_mode,
        :values => ["Nearest Neighbor", "Smooth(old)"]
      },
      {
        :type => :bool,
        :name => "Dynamic Light",
        :parameter => :light,
      },
      {
        :type => :bool,
        :name => "World machine shader",
        :parameter => :twm_shader
      },
      {
        :type => :bool,
        :name => "Colorblind mode",
        :parameter => :colorblind
      },
    ],
    "UI" => [
      {
        :type => :bool,
        :name => "In-Game Timer",
        :parameter => :in_game_timer
      },
      {
        :type => :enum,
        :name => "Language",
        :parameter => :language,
        :values => Language::LANGUAGES
      },
      {
        :type => :enum,
        :name => "FastTravel UI",
        :parameter => :fasttravel_ui,
        :values => ["Original", "WME (WIP)"]
      },
    ],
    "Gameplay" => [
      {
        :type => :enum,
        :name => "Default movement",
        :parameter => :movement,
        :values => ["Walk", "Run"]
      },
      {
        :type => :bool,
        :name => "Skip Text (R)",
        :parameter => :skip_text
      },
      {
        :type => :bool,
        :name => "Purple message box for Entity",
        :parameter => :en_purple_messagebox
      },
      {
        :type => :bool,
        :name => "Enforce april fools",
        :parameter => :enforce_april_fools
      },
      {
        :type => :bool,
        :name => "True Memory Mode",
        :parameter => :true_memory_mode
      },
      {
        :type => :bool,
        :name => "Freeware Mode(!)",
        :parameter => :oneshot_mode
      },
      {
        :type => :bool,
        :name => "World machine shader on Entity footprints",
        :parameter => :twm_shader_footprint
      },
    ],
    "Controls" => [
      {
        :type => :bool,
        :name => 'Control LED lighting on gamepads',
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
      #{ :type => :sep, :name => "Other" },
      {
        :type => :action,
        :name => "Reset Controls",
        :action => Settings.method(:reset_controls!)
      },
    ],
    "Advanced" => [
      {
        :type => :bool,
        :name => "Crashlog privacy",
        :parameter => :crashlog_privacy
        
      },
    ],
    "Debug" => [
      {
        :type => :bool,
        :name => "Debug mode(!)",
        :parameter => :debug
      },
      {
        :type => :bool,
        :name => "Show debug character",
        :parameter => :debug_character
      },
      {
        :type => :bool,
        :name => "Draw debug text in main menu",
        :parameter => :debug_text_scene_title
      },
      {
        :type => :bool,
        :name => "Show debug text",
        :parameter => :debug_text
      },
      {
        :type => :bool,
        :name => "Show picture names",
        :parameter => :debug_picture_names
      },
      {
        :type => :bool,
        :name => "Debug lightmap",
        :parameter => :debug_lightmap
      },
      { :type => :sep },
      {
        :type => :key,
        :name => "Debug",
        :parameter => :controls_debug,
        :bind => Input::DEBUGACTION
      },
      { :type => :sep },
      {
        :type => :action,
        :name => "Clear image cache",
        :action => Proc.new { RPG::Cache.clear }
      }
    ],
  }
end
