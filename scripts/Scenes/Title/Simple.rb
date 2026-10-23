#PRIORITY 1

class Scene_Title
  class Simple < Sprite
    def initialize(overlay_viewport)
      super(overlay_viewport)
      @viewport = overlay_viewport

      self.bitmap = RPG::Cache.title("normal")
      self.order = -2
      self.scale_x = self.scale_y = 2.0

      $game_system.bgm_play($data_system.title_bgm)
    end

    def redraw_bgs
      px = (Graphics.width / 1920.0)
      py = (Graphics.height / 1080.0)

      self.origin_x = self.bitmap.width / 2 * (0.5 * (1.0 - px) + px)
      self.origin_y = self.bitmap.height / 2 * (1.3 * (1.0 - py) + py)
      self.x = Graphics.width / 2
      self.y = Graphics.height / 2
    end

    def update
      #aflsigkjfgsljk;dgsj;lkdsalkj;asdfklj;dsafl;ksadg;ljasdgjh;jfghljh;adsgljhfsgkjhfsagkjh;sfgaj;h
    end
  end
end