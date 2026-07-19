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
        Graphics.fullscreen = $console = value
      end
      def resolution(value)
        if (!Graphics::RESOLUTION_OVERRIDDEN)
          res_data = Graphics::RESOLUTIONS&.[](value) || {:width => 480, :height => 640}
          old_width = Graphics.width
          old_height = Graphics.height
          new_width = res_data[:width]
          new_height = res_data[:height]
          Graphics.move_screen(Graphics.x - (new_width - old_width) / 2, Graphics.y - (new_height - old_height) / 2)
          Graphics.resize_screen(new_width, new_height)
        end
      end
      def colorblind(value)
        $game_switches[252] = value
      end

      # UI
      def in_game_timer(value)
        $game_temp.igt_timer_visible = value
        if $scene.is_a?(Scene_Map) && $scene&.in_game_timer
          $scene.in_game_timer.visible = $game_temp.igt_timer_visible
        end
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
