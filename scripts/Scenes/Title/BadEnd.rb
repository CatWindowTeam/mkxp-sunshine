#PRIORITY 1

class Scene_Title
  class BadEnd < Sprite
    def initialize(overlay_viewport)
      super(overlay_viewport)
      @viewport = overlay_viewport

      self.bitmap = RPG::Cache.title("badend")
      self.order = -2
      self.scale_x = self.scale_y = 2.0
      Audio.bgm_play("Audio/BGM/MyBurdenIsDead")
    end

    def redraw_bgs
      self.bitmap.dispose
      self.bitmap = RPG::Cache.title("badend")

      self.origin_x = self.bitmap.width / 2
      self.origin_y = self.bitmap.height / 2
      self.x = Graphics.width / 2
      self.y = Graphics.height / 2
    end

    def update
      #aflsigkjfgsljk;dgsj;lkdsalkj;asdfklj;dsafl;ksadg;ljasdgjh;jfghljh;adsgljhfsgkjhfsagkjh;sfgaj;h
    end
  end
end