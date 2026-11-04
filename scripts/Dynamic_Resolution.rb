module Graphics
  def self.adapted_file(path, extension = ".png")
    if Settings[:replace_all_niko]
      path = "Graphics/Faces/niko"
    end
    file_tag = RESOLUTIONS[Settings[:resolution] || 999]&.[](:file_tag) || ""
    
    return path + file_tag if File.exist?(path + file_tag + extension)
    path
  end
end
