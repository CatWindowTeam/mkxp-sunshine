module Settings
  module Setters
    class << self
      # these setters apply settings to game.
      # if they are not present, they will still be saved,
      # but changing them in game will not do anything
      # if you do not take the value from settings directly.

      # name must be same as the parameter identifier

      def crashlog_privacy(value)
        Sunshine.crash_privacy = value
      end
      def master_volume(value)
        Audio.master_volume = value / 100.0
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
        Graphics.smooth = value != 0
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

      def invalid_param_checks(value)
        Sunshine.set_sdl_hint("SDL_HINT_INVALID_PARAM_CHECKS", value ? "1" : "2")
      end

      VSYNC_MODES = [1, -1, 0]
      def vsync(value)
        Graphics.setVsync(VSYNC_MODES[value])
      end
	  
      def shutdown_dbus_on_quit(value)
        Sunshine.set_sdl_hint("SDL_HINT_SHUTDOWN_DBUS_ON_QUIT", value ? "1" : "0")
      end

      def profiler(value)
        Profiler.set(value)
      end

      WALLPAPER_MODES = ["normal", "fallback", "disbled"]
      def wallpaper_mode(value)
        Sunshine.wallpaper_mode = WALLPAPER_MODES[value]
      end
    end
  end
end
