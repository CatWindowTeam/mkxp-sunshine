Graphics.frame_rate = 1000

c = 0
limit = 1000

until c >= limit
  c += 1
  Font.default_size = c
  Graphics.update
end
