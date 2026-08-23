c = 0
Graphics.frame_rate = 1000
while c < 1000
	c = c + 1
	Font.default_size = c
	puts CTime.day
	puts CTime.hour
	Graphics.update
	puts c
end
