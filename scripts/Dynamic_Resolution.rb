module Graphics
# 0 - 3:2      - 720x480
# 1 - 4:3      - 640x480 (default)
# 2 - 16:9     - 960x540
# 3 - 16:10    - 960x600
  RESOLUTIONS = [
    {
      :display_name => "4:3 - 640x480",
      :file_tag => "_4_3",
      :width => 640,
      :height => 480,
    },
    {
      :display_name => "3:2 - 720x480",
      :file_tag => "_3_2",
      :width => 720,
      :height => 480,
    },
    {
      :display_name => "16:9 - 960x540",
      :file_tag => "_16_9",
      :width => 960,
      :height => 540,
    },
    {
      :display_name => "16:10 - 960x600",
      :file_tag => "_16_10",
      :width => 960,
      :height => 600,
    },
  ]

  def self.resolutions_names_list
    list = []
    RESOLUTIONS.each do |res_data|
      list << res_data[:display_name]
    end
    list
  end

  def self.adapted_file(path, extention = ".png")
    file_tag = RESOLUTIONS[Settings[:resolution] || 999]&.[](:file_tag) || ""
    if File.exist?(path + file_tag + extention)
      path + file_tag
    end
    path
  end
end