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
  puts Sunshine::SDLVersion_major
  puts Sunshine::SECURITYSTATE
  puts Sunshine::DEVBUILD
  count = 0
  while count <= 999999 do
  	  count.clone
      count = count + 1
      puts count
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
  end
rescue Errno::ENOENT
  filename = $!.message.sub("No such file or directory - ", "")
  print("Unable to find file #{filename}.")
end
