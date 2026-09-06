# Persistent data independent from save file.
# Stores settings and the like.
class LanguageCode
  attr_accessor :full, :lang, :region

  def initialize(str)
    if str.nil?
      str = 'en'
    end
    str = str.to_s
    str = str.strip if str.respond_to?(:strip)
    if str.length == 0
      str = 'en'
    end
    parts = str.split(/[_-]/)
    if parts.nil? || parts.length == 0
      str = 'en'
      parts = str.split(/[_-]/)
    end
    
    if parts.length == 1
      @lang = parts[0].downcase
      @region = nil
      @full = @lang
    elsif parts.length == 2
      @lang = parts[0].downcase
      @region = parts[1].upcase
      full_str = parts[0].downcase + '_' + parts[1].upcase
      @full = full_str
    else
      raise "malformed language code: #{str}"
    end
  end
end

class Persistent
  attr_reader :lang
  @@file_name = nil
  def initialize
    steam_lang = nil
    oneshot_lang = nil
    
    begin
      if defined?(Steam) && defined?(Steam::LANG)
        steam_lang = Steam::LANG
      end
    rescue
      steam_lang = nil
    end
    
    begin
      if defined?(Oneshot) && defined?(Oneshot::LANG)
        oneshot_lang = Oneshot::LANG
      end
    rescue
      oneshot_lang = nil
    end
    
    lang_code = nil
    if steam_lang && steam_lang.to_s.length > 0
      lang_code = steam_lang
    elsif oneshot_lang && oneshot_lang.to_s.length > 0
      lang_code = oneshot_lang
    else
      lang_code = 'en'
    end
    
    begin
      self.lang = LanguageCode.new(lang_code)
    rescue
      # Fallback
      self.lang = LanguageCode.new('en')
    end
    
    begin
      if File.exist?("zh_CN.ver")
        self.lang = LanguageCode.new("zh_CN")
      end
    rescue
      # ignore
    end
    
    begin
      if defined?(Steam) && !Steam.enabled?
        self.lang = LanguageCode.new("en")
      end
    rescue
      # ignore
    end
  end

  def lang=(val)
    
    if val.nil?
      @lang = LanguageCode.new('en')
    elsif val.is_a?(LanguageCode)
      @lang = val
    else
      @lang = LanguageCode.new(val)
    end
    
    begin
      if defined?(Language)
        Language.set(@lang)
      end
    rescue => e
      print("error: " + e.message)
    end
  end
  
  def langcode
    begin
      return self.lang.full.to_s
    rescue
      return 'en'
    end
  end

  # MARSHAL
  def marshal_dump
    [@lang]
  end

  def marshal_load(array)
    begin
      if array && array.length > 0 && array[0]
        self.lang = array[0]
      else
        self.lang = LanguageCode.new('en')
      end
    rescue
      self.lang = LanguageCode.new('en')
    end
  end

  FILE_NAME = nil
  
  def initialize_file_name
    begin
      if defined?(Oneshot) && defined?(Oneshot::SAVE_PATH)
        @@file_name = Oneshot::SAVE_PATH + '/persistent.dat'
      else
        @@file_name = './persistent.dat'
      end
    rescue
      @@file_name = './persistent.dat'
    end
  end
  
  def save
    begin
      initialize_file_name if @@file_name.nil?
      
      File.open(@@file_name, 'wb') do |file|
        Marshal.dump($persistent, file)
      end
    rescue => e
      print("Saving error: " + e.message)
    end
  end

  def self.load
    begin
      file_name = nil
      
      begin
        if defined?(Oneshot) && defined?(Oneshot::SAVE_PATH)
          file_name = Oneshot::SAVE_PATH + '/persistent.dat'
        else
          file_name = './persistent.dat'
        end
      rescue
        file_name = './persistent.dat'
      end
      
      if File.exist?(file_name)
        begin
          File.open(file_name, 'rb') do |file|
            $persistent = Marshal.load(file)
          end
        rescue
          $persistent = Persistent.new
        end
      else
        $persistent = Persistent.new
      end
    rescue => e
      print("Error: " + e.message)
      $persistent = Persistent.new
    end
  end
end
