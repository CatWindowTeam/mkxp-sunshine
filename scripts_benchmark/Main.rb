c = 0
Graphics.frame_rate = 1000
while c < 1000
	c = c + 1
	Font.default_size = c
	Graphics.update
	Logger.Info c
	Logger.Info "test"
end
