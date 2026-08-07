# Engine benchmark idk, check speed via linux time utility

begin
  RPG::Mod.exec_hooks("test", binding)
  Audio.bgm_play("Audio/BGM/MyBurdenIsLight.ogg", Audio.bgm_volume, 100)
    
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
  

  Graphics.setVsync(0)
  Graphics.setVsync(1)
  Graphics.setVsync(-1)
  Graphics.smooth = true
  Graphics.smooth = false
  Graphics.frame_reset
  Graphics.update
  Oneshot.shake
  Sunshine.crashprivacy=true
  Wallpaper.reset
  Input.set_led(255, 150, 30)
  Graphics.freeze
  Wallpaper.reset
  File.exist?("oneshot")
  Oneshot.exiting false
  Oneshot.exiting true
  Audio.bgm_fade(800)
  Audio.bgs_fade(800)
  Audio.me_fade(800)
  Audio.se_play('Audio/SE/title_decision.wav')
  
rescue Errno::ENOENT
  filename = $!.message.sub("No such file or directory - ", "")
  print("Unable to find file #{filename}.")
end
