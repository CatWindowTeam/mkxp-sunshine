module Settings
  FILE_PATH = [Oneshot::SAVE_PATH, 'settings.sunshine.dat'].join '/'

  class << self
    def [](k)
      @data[k]
    end

    def []=(k, v)
      if @data[k] == false or @data[k] == true
        if not (v == false or v == true)
          return
        end
      elsif @data[k] == NilClass
        return
      elsif @data[k].class != v.class
        return
      end

      @data[k] = v;
    end

    def load!
      #Settings::Legacy.load!
      Settings.reset!

      if FileTest.exist?(Settings::FILE_PATH)
        File.open(Settings::FILE_PATH, "rb") do |f|
          settings = Marshal.load(f) rescue return
          if settings.class == Hash
            settings.each do |key, value|
              Settings[key] = value;
            end
          end
        end
      end
      
      Settings.save!
    end

    def save!
      #Settings::Legacy.save!

      if @data
        File.open(Settings::FILE_PATH, "wb") do |f|
          f.write Marshal.dump(@data)
        end
      end
    end
  end

  #module Legacy
  #  FILE_PATH = [Oneshot::SAVE_PATH, 'settings.conf'].join '/'
  #
  #  def self.load!
  #    if not FileTest.exist?(Settings::Legacy::FILE_PATH)
  #      Settings::Legacy.save!
  #      return
  #    end
  #
  #    File.foreach(Settings::Legacy::FILE_PATH) do |line|
  #      if not line.include? "="
  #        next
  #      end
  #      
  #      key, value = line.strip.split("=", 2)
  #      puts ([key, value].join '=')
  #      case key
  #        when "bgm_volume"
  #          Audio.bgm_volume = value.to_i
  #        when "sfx_volume"
  #          Audio.sfx_volume = value.to_i
  #        when "fullscreen"
  #          Graphics.fullscreen = (value.casecmp "true") == 0
  #        when "default_run"
  #          $game_switches[251] = (value.casecmp "true") == 0
  #        when "colorblind_mode"
  #          $game_switches[252] = (value.casecmp "true") == 0
  #        when "automash_enabled"
  #          $game_switches[253] = (value.casecmp "true") == 0
  #        when "frameskip"
  #          Graphics.frameskip = (value.casecmp "true") == 0
  #        when "in_game_timer"
  #          $game_temp.igt_timer_visible = (value.casecmp "true") == 0
  #          $scene.in_game_timer.visible = $game_temp.igt_timer_visible if $scene.is_a?(Scene_Map)
  #      end
  #    end
  #  end
  #
  #  def self.save!
  #    $persistent.save
  #    File.open(Settings::Legacy::FILE_PATH, 'w') do |f|
  #      f.puts (["bgm_volume",       Audio.bgm_volume].join '=')
  #      f.puts (["sfx_volume",       Audio.sfx_volume].join '=')
  #      f.puts (["fullscreen",       Graphics.fullscreen].join '=')
  #      f.puts (["default_run",      ($game_switches[251])].join '=')
  #      f.puts (["colorblind_mode",  ($game_switches[252])].join '=')
  #      f.puts (["automash_enabled", ($game_switches[253])].join '=')
  #      f.puts (["frameskip",        Graphics.frameskip].join '=')
  #      f.puts (["in_game_timer",    $game_temp.igt_timer_visible].join '=')
  #    end
  #  end
  #end
end
