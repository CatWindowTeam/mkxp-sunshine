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
        :graphics_lvl   => 2,

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

        # Advanced
        :frameskip      => false,
        :oneshot_mode   => false,

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
        :type => :enum,
        :name => tr('Graphics Level(WIP)'),
        :parameter => :graphics_lvl,
        :values => [tr("Low"), tr("Normal"), tr("High")]
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
    ],
    tr("Controls") => [
      {
        :type => :base,
        :name => tr('Configure controls'),
        :default => tr("Press F1")
      },
    ],
    tr("Advanced") => [
      {
        :type => :bool,
        :name => tr('Frameskip'),
        :parameter => :frameskip
      },
      {
        :type => :bool,
        :name => tr('Freeware Mode(!)'),
        :parameter => :oneshot_mode
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
