class Voiceover
  class << self
    def set(lc)
      @data = nil
      @tr = nil
      [lc.full, lc.lang].each do |name|
        path = "Audio/Voiceover/#{name}.vo"
        if PhysFS.exist?(path)
          load_voiceover(path)
          break
        end
      end
    end
	  
    def unescape_string(str)
      unescaped = []
      string_began = false
      escape = false
      str.chars.each do |c|
        if not string_began
          string_began = true if c == '"'
          next
        end

        next if c == "\n"
        next if c == "\r"

        if not escape
          break if c == '"'
          if c == '\\'
          	escape = true
            next
          end
        else
          escape = false
          case c
          when 'n'
            unescaped.push("\n")
          when 'r'
            unescaped.push("\r")
          else
            unescaped.push(c)
          end
          next
        end
        unescaped.push(c)
      end
      return unescaped.join('')
    end

    def load_voiceover(path)
      msgid = nil
      msgstr = nil
      @data = Hash.new
      lastLineWasMsgId = false
      lastLineWasMsgStr = false
      if PhysFS.exist?(path)
        PhysFS.read(path).each_line do |line|
          if line.start_with?("msgid ")
            line = line[6..-1]
            #unescape the string
			#note that I tried using undump here instead before, but it doesn't play nicely with non-ascii characters
            msgid = unescape_string(line)
            lastLineWasMsgId = true
            lastLineWasMsgStr = false
          elsif line.start_with?("file ")
            line = line[7..-1]
            #unescape the string
            msgstr = unescape_string(line)
            lastLineWasMsgId = false
            lastLineWasMsgStr = true
          elsif line.start_with?("\"")
            if lastLineWasMsgId
              msgid += unescape_string(line)
              lastLineWasMsgId = true
              lastLineWasMsgStr = false
            elsif lastLineWasMsgStr
              msgstr += unescape_string(line)
              lastLineWasMsgId = false
              lastLineWasMsgStr = true
            else #ignore
              lastLineWasMsgId = false
              lastLineWasMsgStr = false
            end
          else
            lastLineWasMsgId = false
            lastLineWasMsgStr = false
            if !(msgid.nil? || msgid.empty?)
              @data[Oneshot::crc32(msgid)] = msgstr
              msgid = nil
              msgstr = nil
            end
          end
        end
      end
    
      # make sure we cleared out the last one stored
      if !(msgid.nil? || msgid.empty?)
        @data[Oneshot::crc32(msgid)] = msgstr
        msgid = nil
        msgstr = nil
      end
    end
  end
end
