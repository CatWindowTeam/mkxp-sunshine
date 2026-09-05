#PRIORITY 100
# ^ use this for loading order

# Classes for translating text similar to GNU gettext
# Translator class: translate text to another language

# for debug REMOVE BEFORE COMMITTING / BUILDING
# require "zlib"

class Language
  FONT_WESTERN = 'Terminus (TTF)'
  FONT_J = 'HigashiOme Gothic regular'
  LANGUAGES = []
  class << self
    def set(lc)
      @data = nil
      @tr = nil
      [lc.full, lc.lang].each do |name|
        path = "Languages/#{name}.po"
        if File.exist?(path)
          load_pot(path)
          loadFontMap
          Font.default_name = @languageFontMap[name]
          Journal.setLang(name)
          break
        end
      end
      reset_fonts(@text_sprites)
      Oneshot.set_yes_no(tr('Yes'), tr('No'))
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

      unescaped.join('')
    end

    def load_pot(path)
      msgid = nil
      msgstr = nil
      @data = Hash.new
      lastLineWasMsgId = false
      lastLineWasMsgStr = false
      if File.exist?(path)
        File.readlines(path, encoding: "UTF-8").each do |line|
          if line.start_with?("msgid ")
            line = line[6..-1]
            #unescape the string
			#note that I tried using undump here instead before, but it doesn't play nicely with non-ascii characters
            msgid = unescape_string(line)
            lastLineWasMsgId = true
            lastLineWasMsgStr = false
      
          elsif line.start_with?("msgstr ")
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

    # Translate some text
    def tr(string)
      return string unless @data
      s = string.to_s.encode("UTF-8")
      rv = @data[Oneshot::crc32(s)] || s
      rv.nil? ? "NULL" : String.new(rv)
    end

    def loadFontMap
      if !@fontMapLoaded
        @languageFontMap = Hash.new
        path = "Languages/language_fonts.ini"
        if File.exist?(path)
          File.readlines(path, encoding: "UTF-8").each do |line|
            parts = line.split("=", 2)
            #print(parts[0].inspect)
            if parts.length == 2
              LANGUAGES.push(parts[0])
              @languageFontMap[parts[0]] = parts[1].strip
            end
          end
        end
        @fontMapLoaded = true
      end
    end

    # Turn all database items into translatable strings
    def initialize_database
      $data_actors.each do |i|
        i.name = TrString.new(i.name) if i && !i.name.empty?
      end
      $data_items.each do |i|
        if i && !i.name.empty?
          i.name = TrString.new(i.name)
          i.description = TrString.new(i.description)
        end
      end
    end

    def register_text_sprite(key, spr)
      if @text_sprites.nil?
        @text_sprites = Hash.new()
      end
      @text_sprites[key] = spr
      # we dont want memory leak
      @text_sprites.delete_if do |_, ispr|
        ispr.disposed?
      end
    end

    def reset_fonts(sprites)
      if sprites.kind_of?(Array)
        sprites.each do |spr|
          if spr.kind_of?(Sprite) and !spr.disposed?
            spr.bitmap.font.name = Font.default_name
          end
        end
      elsif sprites.kind_of?(Hash)
        sprites.each_value do |spr|
          if spr.kind_of?(Sprite) and !spr.disposed?
            spr.bitmap.font.name = Font.default_name
          elsif spr.kind_of?(Bitmap) and !spr.disposed?
            spr.font.name = Font.default_name
          end
        end
      elsif sprites.kind_of?(Sprite) and not sprites.disposed?
        sprites.bitmap.font.name = Font.default_name
      end
    end
  end
end

# Translatable string
class TrString
  def initialize(str)
    @str = str
  end

  def to_str
    Language.tr(@str)
  end
  alias :to_s :to_str

  def length
    to_str.length
  end

  def +(other)
    to_str + other
  end
end

def tr(text)
  TrString.new(text)
end
