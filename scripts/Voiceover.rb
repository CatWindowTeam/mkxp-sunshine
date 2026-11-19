class Voiceover
  class << self
  	@current_playback = nil
    def set(lang)
		path = "Voiceover/#{lang}/index.txt"
		if PhysFS.exist?(path)
			load_voiceover(path)
			$settings_content.enable_setting(1, 4)
		else
			$settings_content.disable_setting(1, 4)
		end
    end

    def load_voiceover(path)
	  @data = {}
	  
      msgid = nil
      file = nil
      @data = Hash.new

      PhysFS.read(path).each_line do |line|
        if line.start_with?("msgid ")
          line = line[6..-1]
          msgid = Language.unescape_string(line)
        elsif line.start_with?("file ")
          line = line[5..-1]
          #unescape the string
          file = Language.unescape_string(line)
          lastLineWasMsgId = false
          lastLineWasFile = true
        else
          if !(msgid.nil? || msgid.empty?)
            @data[Oneshot::crc32(msgid)] = file
            msgid = nil
            file = nil
          end
        end
      end
    
      # make sure we cleared out the last one stored
      if !(msgid.nil? || msgid.empty?)
        @data[Oneshot::crc32(msgid)] = file
        msgid = nil
        file = nil
      end
    end

    # Translate some text
    def get_file(string)
      return nil unless @data
      rv = @data[Oneshot::crc32(string.to_s.encode("UTF-8"))] || nil

      return rv.nil? ? nil : String.new(rv)
  	end

	def play_voiceover(result_path)
	  if @current_playback
	    @current_playback.stop
	    Audio.unload(@current_playback.path)
	    @current_playback = nil
	  end
	  
	  return if result_path == nil
	  @current_playback = Audio.create_sound(result_path)
	  @current_playback.play
	end
  end
end
