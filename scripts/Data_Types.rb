class Vec2i
  attr_accessor :x
  attr_accessor :y

  def initialize(x = 0, y = 0)
    @x = x
    @y = y
  end

  def abs
    Vec2i.new(@x.abs, @y.abs)
  end
  def abs!
    @x = @x.abs
    @y = @y.abs
  end

  def magnitute
    Math.sqrt(@x * @x + @y * @y)
  end

  def distance_to(other)
    if other.is_a? Integer
      Vec2i.new(
        @x - other,
        @y - other
      ).magnitute
    elsif other.is_a? Float
      Vec2i.new(
        @x - other.to_i,
        @y - other.to_i
      ).magnitute
    elsif other.is_a? Vec2i
      Vec2i.new(
        @x - other.x,
        @y - other.y
      ).magnitute
    else
      raise "Impossible to get distance to an object of type #{other.class} to #{self.class}"
    end
  end

  def +(other)
    if other.is_a? Integer
      Vec2i.new(
        @x + other,
        @y + other
      )
    elsif other.is_a? Float
      Vec2i.new(
        @x + other.to_i,
        @y + other.to_i
      )
    elsif other.is_a? Vec2i
      Vec2i.new(
        @x + other.x,
        @y + other.y
      )
    else
      raise "Impossible to add an object of type #{other.class} to #{self.class}"
    end
  end
  
  def -(other)
    if other.is_a? Integer
      Vec2i.new(
        @x - other,
        @y - other
      )
    elsif other.is_a? Float
      Vec2i.new(
        @x - other.to_i,
        @y - other.to_i
      )
    elsif other.is_a? Vec2i
      Vec2i.new(
        @x - other.x,
        @y - other.y
      )
    else
      raise "Impossible to subtract an object of type #{other.class} to #{self.class}"
    end
  end

  def *(other)
    if other.is_a? Integer
      Vec2i.new(
        @x * other,
        @y * other
      )
    elsif other.is_a? Float
      Vec2i.new(
        @x * other.to_i,
        @y * other.to_i
      )
    elsif other.is_a? Vec2i
      Vec2i.new(
        @x * other.x,
        @y * other.y
      )
    else
      raise "Impossible to add an object of type #{other.class} to #{self.class}"
    end
  end
  
  def /(other)
    if other.is_a? Integer
      Vec2i.new(
        @x / other,
        @y / other
      )
    elsif other.is_a? Float
      Vec2i.new(
        @x / other.to_i,
        @y / other.to_i
      )
    elsif other.is_a? Vec2i
      Vec2i.new(
        @x / other.x,
        @y / other.y
      )
    else
      raise "Impossible to subtract an object of type #{other.class} to #{self.class}"
    end
  end
end