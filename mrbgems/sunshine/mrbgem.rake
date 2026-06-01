MRuby::Gem::Specification.new('sunshine') do |spec|
  spec.license = 'GPLv2'
  spec.summary = "Bindings for Oneshot: Sunshine"

  spec.objs = Dir.glob("#{dir}/src/*.{c,cpp,m,asm,S}").map { |f| objfile(f.relative_path_from(dir).pathmap("#{build_dir}/%X")) }
end
