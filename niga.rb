tp = TracePoint.new(:call) do |t|
  # Для Ruby-методов:
  next unless t.defined_class.is_a?(Module) # обычно всегда
  puts "CALL #{t.defined_class}##{t.method_id} (line #{t.lineno})"
end

tp.enable
p "idi naxui"
tp.disable
