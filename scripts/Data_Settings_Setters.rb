module Settings
  module Setters
    class << self
      # these setters apply settings to game.
      # if they are not present, they will still be saved,
      # but changing them in game will not do anything
      # if you do not take the value from settings directly.

      # name must be same as the parameter identifier

	  def crashlog_privacy(value)
		Sunshine.crashprivacy=value
	  end
      
      def bgm_volume(value)
        Audio.bgm_volume = value
      end
      def sfx_volume(value)
        Audio.sfx_volume = value
      end
      
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

	  def scaling_mode(value)
		if value == 0
			Graphics.smooth = false
		else 
			Graphics.smooth = true
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
        $persistent.lang = Language::LANGUAGES[value]
        $scene&.redraw
      end

      def movement(value)
        $game_switches[251] = value != 0
      end
      def skip_text(value)
        $game_switches[253] = value != 0
      end

      def frameskip(value)
        Graphics.frameskip = value
      end

      def SDL_HINT_INVALID_PARAM_CHECKS(value)
        if value
          Sunshine.setSDLHint("SDL_HINT_INVALID_PARAM_CHECKS", "1")
        else
          Sunshine.setSDLHint("SDL_HINT_INVALID_PARAM_CHECKS", "2")
        end
      end

      def SDL_HINT_SHUTDOWN_DBUS_ON_QUIT(value)
        if value
          Sunshine.setSDLHint("SDL_HINT_SHUTDOWN_DBUS_ON_QUIT", "1")
        else
          Sunshine.setSDLHint("SDL_HINT_SHUTDOWN_DBUS_ON_QUIT", "0")
        end
      end
    end
  end
end
