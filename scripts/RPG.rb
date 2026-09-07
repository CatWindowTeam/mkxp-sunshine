class Tone
  def +(o)
    Tone.new(self.red + o.red, self.green + o.green, self.blue + o.blue, self.gray + o.gray)
  end

  def *(s)
    Tone.new(self.red * s, self.green * s, self.blue * s, self.gray * s)
  end

  def blank?
    self.red == 0 && self.green == 0 && self.blue == 0 && self.gray == 0
  end
end

module RPG
  module Cache
    def self.autotile(filename)
      if !Settings[:water] && PhysFS.exist?("Graphics/Autotiles/" + filename + "_simple.png")
        filename += "_simple"
      end
      self.load_bitmap("Graphics/Autotiles/", filename)
    end
    def self.character(filename, hue)
	  filename = filename.downcase
	  if ($game_switches[160] || Settings[:true_memory_mode]) && filename.start_with?("niko")
	    filename.gsub!(/niko/, "en")
	  end
      self.load_bitmap("Graphics/Characters/", filename, hue)
    end
    def self.face(filename)
      filename = filename.downcase
      if ($game_temp.message_face != nil && ((CTime.month == 4 && CTime.day == 1) or Settings[:enforce_april_fools]) && filename.start_with?("niko"))
        filename = "af"
      end	  
      if $game_switches[160] && filename.start_with?("niko")
        filename.gsub!(/niko/, "en")
      end
      self.load_bitmap("Graphics/Faces/", filename)
    end
  end
end
