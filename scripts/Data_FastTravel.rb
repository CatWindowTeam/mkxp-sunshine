class FastTravel
  class Zone
    attr_reader :name
    attr_reader :maps
    attr_reader :locations
    def initialize(name, maps, locations)
      @name = name
      @maps = maps
      @locations = locations;
      RPG::Mod.exec_hooks("hooks/FastTravel/init", binding)
    end
  end

  class ZoneLocations
    attr_reader :x
    attr_reader :y
    attr_reader :niko_x
    attr_reader :niko_y
    attr_reader :next_top
    attr_reader :next_right
    attr_reader :next_bottom
    attr_reader :next_left
    def initialize(x, y, niko_x, niko_y, next_top, next_right, next_bottom, next_left)
      @x = x
      @y = y
      @niko_x = niko_x
      @niko_y = niko_y
      @next_top = next_top
      @next_right = next_right
      @next_bottom = next_bottom
      @next_left = next_left
      RPG::Mod.exec_hooks("hooks/ZoneLocations/init", binding)
    end
  end

  ZONES = {
    :red_ground => Zone.new(tr("The Refuge (Surface)"), {
      :ground1 => tr("elevator street"),
      :ground2 => tr("vendor street"),
      :backalley => tr("back alley"),
      :library => tr("library"),
      :factory => tr("factory"),
    }, nil),
    :red => Zone.new(tr("The Refuge"), {
      :garden => tr("garden"),
      :gate => tr("city gate"),
      :elevator => tr("elevator deck"),
      :apartments => tr("apartments"),
      :cafe => tr("cafe"),
      :office => tr("office"),
      :obsdeck => tr("observation deck"),
    }, nil),
    :green => Zone.new(tr("The Glen"), {
      :village => tr("village"),
      :ruins => tr("ruins"),
      :forest => tr("forest"),
      :wall => tr("the gate"),
      :dock => tr("dock"),
      :courtyard => tr("courtyard"),
	  :research => tr("research station"),
	  :grave => tr("graveyard")
    }, nil),
    :blue => Zone.new(tr("The Barrens"), {
      :entrance => tr("entrance"),
      :outpost => tr("outpost"),
      :cliffs => tr("cliffs"),
      :mineshaft => tr("mineshaft entrance"),
      :factory => tr("old factory"),
      :dorms => tr("dormitories"),
      :swamp => tr("shrimp swamp"),
      :docks => tr("docks"),
      :quarry => tr("lookout point"),
    },
    {
      :entrance => ZoneLocations.new(-81, 22, -4, 3, :docks, :outpost, nil, nil),
      :outpost => ZoneLocations.new(-52, 42, 6, -3, nil, :cliffs, nil, :entrance),
      :cliffs => ZoneLocations.new(4, -3, 21, 38, :factory, :quarry, nil, :outpost),
      :mineshaft => ZoneLocations.new(88, -3, 5, 0, nil, nil, nil, :quarry),
      :factory => ZoneLocations.new(15, -42, 13, 2, nil, nil, :cliffs, :dorms),
      :dorms => ZoneLocations.new(-46, -49, 20, 4, nil, :factory, nil, :swamp),
      :swamp => ZoneLocations.new(-98, -45, 10, 7, nil, :dorms, :docks, nil),
      :docks => ZoneLocations.new(-82, -9, -4, -2, :swamp, nil, :entrance, nil),
      :quarry => ZoneLocations.new(52, -4, 17, -3, nil, :mineshaft, nil, :cliffs),
    }),
  }
end
