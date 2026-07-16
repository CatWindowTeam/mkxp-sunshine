module Settings
  FILE_PATH = [Oneshot::SAVE_PATH, 'settings.sunshine.dat'].join '/'

  class << self
    def [](k)
      @data&.[](k)
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

    def has?(parameter)
      @data.has_key?(parameter)
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
end
