N = 5_000_000
sink = 0

# 1. Цикл, арифметика и ветвления
i = 0
while i < N
  sink ^= (i * 31 + 17) % 1_000_003
  i += 1
end

# 2. Вызовы методов
class TestObject
  def initialize(value)
    @value = value
  end

  def calculate(x)
    ((@value + x) * 3) ^ 0x55aa
  end
end

objects = Array.new(10) { |x| TestObject.new(x) }

i = 0
while i < N
  sink ^= objects[i % objects.length].calculate(i)
  i += 1
end

# 3. Создание объектов и сборка мусора
i = 0
while i < N
  data = {
    id: i,
    name: "object_#{i}",
    values: [i, i * 2, i * 3],
    enabled: (i & 1) == 0
  }

  sink ^= data[:values][1]
  i += 1
end

# 4. Строковые операции
text = "benchmark_string_" * 20

i = 0
while i < N
  value = "#{text}:#{i}".upcase.reverse
  sink ^= value.length
  i += 1
end

# 5. Массивы
array = Array.new(10_000) { |x| x }

i = 0
while i < N
  array[i % array.length] = (array[i % array.length] * 31 + i) % 1_000_003
  sink ^= array[i % array.length]
  i += 1
end

# 6. Hash lookup
hash = {}
10_000.times do |x|
  hash["key_#{x}"] = x
end

i = 0
while i < N
  sink ^= hash["key_#{i % 10_000}"]
  i += 1
end

# 7. BigInt-арифметика
big = (1 << 100_000) + 12_345

200.times do
  big = big * big
  big %= (1 << 100_000) - 1
  sink ^= big & 0xffff
end

# 8. Исключения
exception_count = 100_000
i = 0

while i < exception_count
  begin
    raise RuntimeError, "benchmark error"
  rescue RuntimeError
    sink += 1
  end

  i += 1
end

# 9. clone / dup
object = {
  name: "test",
  values: Array.new(100, 123),
  nested: { enabled: true }
}

i = 0
while i < N
  copy = object.dup
  sink ^= copy[:values].length
  i += 1
end

puts "sink=#{sink}"
