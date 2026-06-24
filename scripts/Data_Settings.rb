module Settings
  class << self
    def reset!
      @data = {
        # Audio
        :bgm_volume     => 100,
        :sfx_volume     => 100,

        # Video
        :fullscreen     => false,
        :colorblind     => false,
        :frameskip      => true,
        :twm_shader    => true,

        # UI
        :in_game_timer  => false,
        :language       => 0,
        :fasttravel_ui  => 0,
        :debug_text_scene_title => true,

        # Gameplay
        :movement       => 0,
        :skip_text      => false,
        :en_purple_messagebox => true,
        :enforce_april_fools => false,
        :true_memory_mode => false,
		:pre_solstice_update_content => false,
		:dejavu_mode => false,
		:demo           => false,
		:oneshot_mode   => false,
		
        # Advanced

		#controls
		:gamepad_led    => true,
		
        # Debug
        :debug          => false,
        :debug_character          => false,
        :debug_text          => false,
      }
    end
  end
end

class Window_Settings
  # if parameter is linked to data in class Settings, then default value in this structure is ignored
  DATA = {
    tr("Audio") => [
      {
        :type => :int,
        :name => tr('BGM Volume'),
        :parameter => :bgm_volume,
        :min => 0,
        :max => 100
      },
      {
        :type => :int,
        :name => tr('SFX Volume'),
        :parameter => :sfx_volume,
        :min => 0,
        :max => 100
      },
    ],
    tr("Video") => [
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
        :values => [tr("Original"), tr("WME")]
      },
      {
        :type => :bool,
        :name => tr('Draw debug text in main menu(RR)'),
        :parameter => :debug_text_scene_title
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
        :name => tr('Purple message box for Entity(WIP)'),
        :parameter => :en_purple_messagebox
      },
      {
        :type => :bool,
        :name => tr('Enforce april fools(WIP)'),
        :parameter => :enforce_april_fools
      },
      {
        :type => :bool,
        :name => tr('True Memory Mode(WIP)'),
        :parameter => :true_memory_mode
      },
      {
        :type => :bool,
        :name => tr('Enable Pre-Solstice update content(WIP)'),
        :parameter => :pre_solstice_update_content
      },
      {
        :type => :bool,
        :name => tr('Dejavu mode(WIP)'),
        :parameter => :dejavu_mode
      },
      {
        :type => :bool,
        :name => tr('Demo mode(WIP)'),
        :parameter => :demo
      },
      {
        :type => :bool,
        :name => tr('Freeware Mode(!)'),
        :parameter => :oneshot_mode
      },
    ],
    tr("Controls") => [
      {
        :type => :base,
        :name => tr('Configure controls'),
        :default => tr("Press F1")
      },
      {
        :type => :bool,
        :name => tr('Control LED lighting on gamepads'),
        :parameter => :gamepad_led
      },
    ],
    tr("Advanced") => [
	  {
		:type => :bool,
		:name => tr('useless option:3'),
		:parameter => :erfsdgvfdgfgf
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
        :name => tr('Show debug text'),
        :parameter => :debug_text
      },
    ],
  }
end
