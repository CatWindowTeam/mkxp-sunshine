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

        # UI
        :in_game_timer  => false,
        :language       => 0,

        # Gameplay
        :movement       => 0,
        :skip_text      => false,

        # Advanced
        :frameskip      => false,
        :oneshot_mode   => false,

        # Debug
        :debug          => false,
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
    ],
  }
end