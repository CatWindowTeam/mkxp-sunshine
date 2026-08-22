c = 0
while c < 1000
	c = c + 1
	puts c
	Graphics.frame_reset
	Graphics.update
  	Font.default_size = c
end
