MRuby::Gem::Specification.new('core') do |spec|
  spec.license = 'GPLv2'
  spec.summary = "Bindings for MKXP engine itself"

  spec.objs = Dir.glob("#{dir}/src/*.{c,cpp,m,asm,S}").map { |f| objfile(f.relative_path_from(dir).pathmap("#{build_dir}/%X")) }
end
