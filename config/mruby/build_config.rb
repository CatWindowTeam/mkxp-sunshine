# check for flags here:
#  default configuration: https://github.com/mruby/mruby/blob/master/build_config/default.rb
#  compilation guide: https://github.com/mruby/mruby/blob/master/doc/guides/compile.md

MRuby::Build.new do |conf|
  conf.toolchain

  # our gems
  conf.gem '../../mrbgems/core'
  conf.gem '../../mrbgems/oneshot'
  conf.gem '../../mrbgems/sunshine' # :3

  # core gems
  conf.gem :core => 'mruby-bin-config' # mruby-config would be used in CMakeLists.txt

  if ENV.include?('DEBUG')
    conf.enable_debug
  end
end
