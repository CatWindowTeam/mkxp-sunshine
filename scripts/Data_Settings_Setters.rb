module Settings
  module Setters
    class << self
      # these setters apply settings to game.
      # if they are not present, they will still be saved,
      # but changing them in game will not do anything
      # if you do not take the value from settings directly.

      # name must be same as the parameter identifier
      
      # Audio
      def bgm_volume(value)
        Audio.bgm_volume = value
      end
      def sfx_volume(value)
        Audio.sfx_volume = value
      end
      
      # Video
      def fullscreen(value)
        Graphics.fullscreen = value
      end
      def colorblind(value)
        $game_switches[252] = value
      end

      # UI
      def in_game_timer(value)
        $game_temp.igt_timer_visible = value
        $scene.in_game_timer.visible = $game_temp.igt_timer_visible if $scene.is_a?(Scene_Map)
      end
      def language(value)
        # idk how to set language
        # TODO: fix languages on new ruby versions
      end

      # Gameplay
      def movement(value)
        $game_switches[251] = value != 0
      end
      def skip_text(value)
        # idk how to set skip_text parameter
        # TODO: fix skip_text settings setter
      end

      # Advanced
      def frameskip(value)
        Graphics.frameskip = value
      end
    end
  end
end