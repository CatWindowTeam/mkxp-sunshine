#PRIORITY 1

class Scene_Title
  class TowerAnimation
    MAX_POS = 1550
    TOWER_POS = -MAX_POS - 160
    TOWER_Z_OFFSET = -220
    PARTICLES_COUNT = 2000

    class BG_Particle < Sprite
      DISTANCE = 600.0

      def dist(x1, y1, x2, y2)
        x = x1 - x2
        y = y1 - y2
        Math.sqrt(x*x + y*y)
      end

      def initialize(viewport, bitmap, timer)
        super(viewport)
        @viewport = viewport
        self.bitmap = bitmap
        self.smooth = true
        self.origin_x = 1
        self.origin_y = 1
        self.scale_x = 32
        self.scale_y = 32
        init(timer)
      end

      def update
        return if disposed?
        if !Settings[:title_particles]
          self.opacity = 0
          return
        end

        @pos_x += @speed_x * Graphics.scaled_delta
        @pos_y += @speed_y * Graphics.scaled_delta
        self.x = @pos_x
        self.y = @pos_y
        self.rot_z += @rot_speed * Graphics.scaled_delta
        #173,37,74
        #34,5,41
        update_color
        
        @timer += Graphics.scaled_delta

        @opacity += (@timer > 512 ? -2 : 2) * Graphics.scaled_delta
        @opacity = 255.0 if @opacity > 255.0
        init(0) if @opacity <= 0
        self.opacity = @opacity
      end

      def init(timer)
        self.x = -Graphics.width/2 + rand(0..Graphics.width)
        self.y = -Graphics.height/2 + rand(0..Graphics.height)
        #self.z = @viewport.origin_z
        #self.perspective = true
        self.rot_z = rand(-180..180)
        self.order = -1
        self.opacity = 0
        @opacity = 0

        @rot_speed = rand(-20..20) / 80.0
        @speed_x = rand(-10..10) / 70.0
        @speed_y = rand(-10..10) / 70.0
        @pos_x = self.x
        @pos_y = self.y

        update_color

        @timer = -timer
      end

      def update_color
        dist = dist(@pos_x, @pos_y, -Graphics.width/6, 0)
        if dist < DISTANCE
          self.color.set(lerp(173, 34, dist / DISTANCE), lerp(37, 5, dist / DISTANCE), lerp(74, 41, dist / DISTANCE))
        else
          dist -= DISTANCE
          self.color.set(lerp(34, 0, dist / DISTANCE), lerp(5, 0, dist / DISTANCE), lerp(41, 0, dist / DISTANCE))
        end
      end
    end

    def initialize(overlay_viewport)
      @animation_ended = false

      @timer = 0
      @speed = 0.01
      @distance = 0.0
      @speed_down = false

      @loaded_sound = Audio.create_sound("Audio/loaded", false, Audio.sounds_group)

      @viewport = Viewport.new(0, 0, Graphics.width, Graphics.height)
      @viewport.origin_x = -Graphics.width / 2
      @viewport.origin_y = -Graphics.height / 2 - MAX_POS
      @viewport.origin_z = 200
      @viewport.rot_x = -30
      @viewport.order = -1

      @overlay_viewport = overlay_viewport
      @overlay_viewport.origin_y = -MAX_POS

      # main bg
      
      @background_sprite = Sprite.new(@viewport)
      @background_sprite.bitmap = RPG::Cache.title("normal_bg")
      @background_sprite.scale_x = 2.2
      @background_sprite.scale_y = 2.0
      @background_sprite.perspective = true
      @background_sprite.z = @viewport.origin_z
      @background_sprite.order = -2
      
      @background_sprite_shadow = Sprite.new(@viewport)
      @background_sprite_shadow.bitmap = RPG::Cache.title("normal_back_shadow")
      @background_sprite_shadow.scale_x = @background_sprite_shadow.scale_y = 2.0
      @background_sprite_shadow.perspective = true
      @background_sprite_shadow.z = @viewport.origin_z
      
      @background_sprite_water_back = Sprite.new(@viewport)
      @background_sprite_water_back.bitmap = Bitmap.new(412, 388)
      @background_sprite_water_back.bitmap.stretch_blt(Rect.new(0, 0, 412, 388), RPG::Cache.title("tower"), Rect.new(0, 240, 412, 388))
      @background_sprite_water_back.scale_x = @background_sprite_water_back.scale_y = 2.0
      @background_sprite_water_back.perspective = true
      @background_sprite_water_back.z = @viewport.origin_z
      @background_sprite_water_back.origin_x = 206
      @background_sprite_water_back.origin_y = 194
      @background_sprite_water_back.rot_x = 90
      #@background_sprite_water_back.smooth = true
      
      @background_sprite_water_light = Sprite.new(@viewport)
      @background_sprite_water_light.bitmap = RPG::Cache.title("normal_water_light")
      @background_sprite_water_light.scale_x = @background_sprite_water_light.scale_y = 2.0
      @background_sprite_water_light.perspective = true
      @background_sprite_water_light.z = @viewport.origin_z
      @background_sprite_water_light.blend_type = 1
      
      @background_sprite_niko = Sprite.new(@viewport)
      @background_sprite_niko.bitmap = RPG::Cache.title("normal_niko")
      @background_sprite_niko.scale_x = @background_sprite_niko.scale_y = 2.0
      @background_sprite_niko.perspective = true
      @background_sprite_niko.z = @viewport.origin_z
      
      @background_sprite_light = Sprite.new(@viewport)
      @background_sprite_light.bitmap = RPG::Cache.title("normal_light")
      @background_sprite_light.scale_x = @background_sprite_light.scale_y = 2.0
      @background_sprite_light.perspective = true
      @background_sprite_light.z = @viewport.origin_z
      @background_sprite_light.blend_type = 1
      
      # main bg paricles
      
      @particles = []
      @particle_bitmap = Bitmap.new(2, 2)
      @particle_bitmap.fill_rect(0, 0, 2, 1, Color.new(255, 255, 255))
      @particle_bitmap.fill_rect(0, 1, 2, 1, Color.new(255, 255, 255, 0))

      (0...2000).each do |i|
        @particles << BG_Particle.new(@viewport, @particle_bitmap, i)
      end

      # tower

      @black = Sprite.new(@viewport)
      @black.bitmap = RPG::Cache.picture("black")
      @black.y = -MAX_POS
      @black.origin_x = @black.bitmap.width / 2
      @black.origin_y = @black.bitmap.height / 2
      @black.order = 2

      atlas = RPG::Cache.title("tower")

      @floor = Sprite.new(@viewport)
      @floor.bitmap = Bitmap.new(192, 192)
      @floor.bitmap.stretch_blt(Rect.new(0, 0, 192, 192), atlas, Rect.new(0, 0, 192, 192))
      @floor.origin_x = @floor.origin_y = 96
      @floor.y = TOWER_POS
      @floor.z = TOWER_Z_OFFSET
      @floor.rot_x = 90
      
      @wall_left = Sprite.new(@viewport)
      @wall_left.bitmap = Bitmap.new(96, 240)
      @wall_left.bitmap.stretch_blt(Rect.new(0, 0, 96, 240), atlas, Rect.new(192, 0, 96, 240))
      @wall_left.origin_y = 240
      @wall_left.x = -96
      @wall_left.y = TOWER_POS
      @wall_left.z = TOWER_Z_OFFSET
      @wall_left.rot_y = 90.0 - 26.57
      @wall_left.scale_x = 1.118033989

      @wall_center = Sprite.new(@viewport)
      @wall_center.bitmap = Bitmap.new(96, 240)
      @wall_center.bitmap.stretch_blt(Rect.new(0, 0, 96, 240), atlas, Rect.new(288, 0, 96, 240))
      @wall_center.origin_x = 48
      @wall_center.origin_y = 240
      @wall_center.z = 96 + TOWER_Z_OFFSET
      @wall_center.y = TOWER_POS

      @wall_right = Sprite.new(@viewport)
      @wall_right.bitmap = Bitmap.new(96, 240)
      @wall_right.bitmap.stretch_blt(Rect.new(0, 0, 96, 240), atlas, Rect.new(384, 0, 96, 240))
      @wall_right.origin_x = 96
      @wall_right.origin_y = 240
      @wall_right.x = 96
      @wall_right.y = TOWER_POS
      @wall_right.z = TOWER_Z_OFFSET
      @wall_right.rot_y = -90.0 + 26.57
      @wall_right.scale_x = 1.118033989

      @mirror_wall_left = Sprite.new(@viewport)
      @mirror_wall_left.bitmap = @wall_left.bitmap
      @mirror_wall_left.origin_y = 240
      @mirror_wall_left.x = -96
      @mirror_wall_left.y = TOWER_POS
      @mirror_wall_left.z = TOWER_Z_OFFSET
      @mirror_wall_left.rot_y = 90.0 - 26.57
      @mirror_wall_left.scale_x = 1.118033989

      @mirror_wall_center = Sprite.new(@viewport)
      @mirror_wall_center.bitmap = @wall_center.bitmap
      @mirror_wall_center.origin_x = 48
      @mirror_wall_center.origin_y = 240
      @mirror_wall_center.z = 96 + TOWER_Z_OFFSET
      @mirror_wall_center.y = TOWER_POS

      @mirror_wall_right = Sprite.new(@viewport)
      @mirror_wall_right.bitmap = @wall_right.bitmap
      @mirror_wall_right.origin_x = 96
      @mirror_wall_right.origin_y = 240
      @mirror_wall_right.x = 96
      @mirror_wall_right.y = TOWER_POS
      @mirror_wall_right.z = TOWER_Z_OFFSET
      @mirror_wall_right.rot_y = -90.0 + 26.57
      @mirror_wall_right.scale_x = 1.118033989

      @mirror_wall_left.order = @mirror_wall_center.order = @mirror_wall_right.order =
      @mirror_wall_left.scale_y = @mirror_wall_center.scale_y = @mirror_wall_right.scale_y = -1

      @pedestal = Sprite.new(@viewport)
      @pedestal.bitmap = Bitmap.new(192, 192)
      @pedestal.bitmap.stretch_blt(Rect.new(0, 0, 48, 32), atlas, Rect.new(144, 208, 48, 32))
      @pedestal.origin_x = 24
      @pedestal.origin_y = 32
      @pedestal.y = TOWER_POS
      @pedestal.z = TOWER_Z_OFFSET

      @lamp = Sprite.new(@viewport)
      @lamp.bitmap = Bitmap.new(32, 32)
      @lamp.bitmap.stretch_blt(Rect.new(0, 0, 32, 32), atlas, Rect.new(0, 0, 32, 32))
      @lamp.scale_x = @lamp.scale_y = 0.5
      @lamp.origin_x = 16
      @lamp.origin_y = 16
      @lamp.blend_type = 1
      @lamp.rot_x = 30
      @lamp.y = TOWER_POS - 28
      @lamp.z = TOWER_Z_OFFSET
      #@lamp.smooth = true
      @lamp.order = 3

      @lamp_light = Sprite.new(@viewport)
      @lamp_light.bitmap = Bitmap.new(64, 64)
      @lamp_light.bitmap.stretch_blt(Rect.new(0, 0, 64, 64), atlas, Rect.new(416, 240, 64, 64))
      @lamp_light.scale_x = @lamp_light.scale_y = 0.5
      @lamp_light.origin_x = 32
      @lamp_light.origin_y = 32
      @lamp_light.blend_type = 1
      @lamp_light.rot_x = 30
      #@lamp_light.smooth = true
      @lamp_light.y = TOWER_POS - 28
      @lamp_light.z = TOWER_Z_OFFSET

      @base_bitmap = Bitmap.new(2, 2)
      @base_bitmap.fill_rect(0, 0, 2, 1, Color.new(10, 10, 16))
      @base_bitmap.fill_rect(0, 1, 2, 1, Color.new(0, 0, 0))
      
      @front_wall_left = Sprite.new(@viewport)
      @front_wall_left.bitmap = @base_bitmap
      @front_wall_left.x = -96
      @front_wall_left.y = TOWER_POS
      @front_wall_left.z = TOWER_Z_OFFSET
      @front_wall_left.rot_y = -90.0 + 26.57
      @front_wall_left.scale_x = 1.118033989 * 48

      @front_wall_center = Sprite.new(@viewport)
      @front_wall_center.bitmap = @base_bitmap
      @front_wall_center.origin_x = 1
      @front_wall_center.z = -96 + TOWER_Z_OFFSET
      @front_wall_center.y = TOWER_POS
      @front_wall_center.scale_x = 48

      @front_wall_right = Sprite.new(@viewport)
      @front_wall_right.bitmap = @base_bitmap
      @front_wall_right.x = 96
      @front_wall_right.y = TOWER_POS
      @front_wall_right.z = TOWER_Z_OFFSET
      @front_wall_right.rot_y = -90.0 - 26.57
      @front_wall_right.scale_x = 1.118033989 * 48

      @front_wall_left.scale_y = @front_wall_center.scale_y = @front_wall_right.scale_y = 240
      @front_wall_left.smooth = @front_wall_center.smooth = @front_wall_right.smooth = true

      @mirror_wall_left.perspective = @mirror_wall_center.perspective = @mirror_wall_right.perspective =
      @front_wall_left.perspective = @front_wall_center.perspective = @front_wall_right.perspective =
      @wall_left.perspective = @wall_center.perspective = @wall_right.perspective =
      @lamp.perspective = @lamp_light.perspective =
      @floor.perspective = @pedestal.perspective = true

      atlas.dispose
      #@wall_left.smooth = @wall_center.smooth = @wall_right.smooth =
      #@floor.smooth = true
      #@loaded_sound.play
    end

    def redraw_bgs
      @viewport.rect.width = Graphics.width
      @viewport.rect.height = Graphics.height
      
      @viewport.origin_x = -Graphics.width / 2
      @viewport.origin_y = -Graphics.height / 2 - MAX_POS + @distance

      px = (Graphics.width / 1920.0)
      py = (Graphics.height / 1080.0)

      @background_sprite.origin_x =
      @background_sprite_shadow.origin_x =
      @background_sprite_water_light.origin_x =
      @background_sprite_niko.origin_x =
      @background_sprite_light.origin_x =
      @background_sprite_niko.bitmap.width / 2 * (0.5 * (1.0 - px) + px)

      @background_sprite.origin_y =
      @background_sprite_shadow.origin_y =
      @background_sprite_water_light.origin_y =
      @background_sprite_niko.origin_y =
      @background_sprite_light.origin_y =
      @background_sprite_niko.bitmap.height / 2 * (1.3 * (1.0 - py) + py)

      @background_sprite_water_back.x = @background_sprite_niko.origin_x - 400
      @background_sprite_water_back.y = @background_sprite_niko.origin_y + 50
    end

    def update
      @background_sprite_water_back.rot_y -= 0.04 * Graphics.scaled_delta
      @particles.each { |particle| particle.update }

      return if tower_disposed?
      if (!@speed_down && Input.pressed_key != nil) || !Settings[:start_animation]
        if @timer < 16
          @loaded_sound.play
        end

        @speed = 0
        @animation_ended = true
        @timer = 48
        @distance = MAX_POS
        $game_system.bgm_play($data_system.title_bgm)

        @viewport.origin_y = -Graphics.height / 2
        @viewport.rot_x = 0
        @overlay_viewport.origin_y = 0
        dispose_tower
        return true
      end
      
      if !@splash_skipped && @timer >= 16
        @black.visible = false
        @lamp.visible = false
        @loaded_sound.play
        @splash_skipped = true
      end

      if @timer < 48
        @timer += Graphics.scaled_delta
        return
      end

      if @speed > 0
        @speed += (@speed_down ? -5.0 : 7.0) * Graphics.scaled_delta
      elsif !@animation_ended
        @speed = 0
        @animation_ended = true
        dispose_tower
      end

      @viewport.origin_y += @speed * 0.1 * Graphics.scaled_delta
      @overlay_viewport.origin_y += @speed * 0.1 * Graphics.scaled_delta
      @distance += @speed * 0.1 * Graphics.scaled_delta
      #puts @distance

      if @speed > 300
        @speed_down = true
        $game_system.bgm_play($data_system.title_bgm)
      end

      @viewport.rot_x = lerp(-30.0, 0.0, @distance / MAX_POS)
    end

    def disposed?
      @viewport.disposed?
    end

    def dispose
      dispose_tower
      @background_sprite.bitmap.dispose
      @background_sprite.dispose
      @background_sprite_shadow.bitmap.dispose
      @background_sprite_shadow.dispose
      @background_sprite_water_back.bitmap.dispose
      @background_sprite_water_back.dispose
      @background_sprite_water_light.bitmap.dispose
      @background_sprite_water_light.dispose
      @background_sprite_niko.bitmap.dispose
      @background_sprite_niko.dispose
      @background_sprite_light.bitmap.dispose
      @background_sprite_light.dispose
      
      @particle_bitmap.dispose
      @particles.each { |particle| particle.dispose }
      
      @viewport.dispose
    end

    def tower_disposed?
      @floor.disposed?
    end

    def dispose_tower
      return if tower_disposed?
      @black.dispose
        
      @floor.bitmap.dispose
      @floor.dispose
        
      @wall_right.bitmap.dispose
      @wall_center.bitmap.dispose
      @wall_left.bitmap.dispose
        
      @wall_right.dispose
      @wall_center.dispose
      @wall_left.dispose
        
      @mirror_wall_right.dispose
      @mirror_wall_center.dispose
      @mirror_wall_left.dispose
        
      @pedestal.bitmap.dispose
      @pedestal.dispose
        
      @lamp.bitmap.dispose
      @lamp.dispose
      @lamp_light.bitmap.dispose
      @lamp_light.dispose
        
      @base_bitmap.dispose
        
      @front_wall_right.dispose
      @front_wall_center.dispose
      @front_wall_left.dispose
    end
  end
end