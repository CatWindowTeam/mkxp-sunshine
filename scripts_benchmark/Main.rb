# Engine benchmark idk, check speed via linux time utility
begin
  RPG::Mod.exec_hooks("test", binding)
  count = 0
  while count <= 500000 do
    puts count
    count += 1
  end
  while count >= 1 do
    puts count
    count -= 1
  end
  
  count = 1.5
  while count <= 99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999 do
    puts count
    count = count * count
  end

  count = 2
  while count <= 99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999 do
    puts count
    count = count * count
  end
  count / 67.1488
  count = 1
  while count <= 5000 do
  	 Graphics.frame_rate = count
     count += 1
  end
  
  count = 1
  while count <= 60 do
  	 Font.default_size = count
     count += 1
  end
 
  Sunshine.crashprivacy=true
  Wallpaper.reset
  Input.set_led(255, 150, 30)
  Graphics.freeze
  Wallpaper.reset
  File.exist?("oneshot")
  Oneshot.exiting false
  Oneshot.exiting true
  puts CTime.month
  puts CTime.day
  puts CTime.hour
  Sunshine.wallpapermode="fallback"
  Sunshine.wallpapermode="disabled"
  Sunshine.wallpapermode="normal"
  puts Sunshine::SDLVersion_major
  puts Sunshine::SDLVersion_minor
  puts Sunshine::SDLVersion_micro
  puts Sunshine::SECURITYSTATE
  puts Sunshine::VERSION
  puts Sunshine::DEVBUILD
  count = 0
  Oneshot.shake
  Oneshot.shake
  Oneshot.shake
  Oneshot.shake
  Oneshot.shake
  while count <= 9999999 do
  	  count.clone
      count = count + 1
      puts count
      Graphics.setVsync(0)
      Graphics.setVsync(1)
      Graphics.setVsync(-1)
      Graphics.smooth = true
      Graphics.smooth = false
      Graphics.frame_reset
      Graphics.update
      begin
      	raise 'Boom!'
      rescue
        puts 'Rescued an exception.'
      end
      begin
        1 / 0 # Raises ZeroDivisionError, a subclass of StandardError.
      rescue
        puts "Rescued #{$!.class}"
      end
      begin
        Dir.open('nosuch')
      rescue Errno::ENOTDIR
        puts "Rescued #{$!.class}"
      rescue Errno::ENOENT
        puts "Rescued #{$!.class}"
      rescue 
      	puts "g"
      end
  end
rescue Errno::ENOENT
  filename = $!.message.sub("No such file or directory - ", "")
  print("Unable to find file #{filename}.")
end
