MRuby::Build.new do |conf|
  conf.toolchain
  
  conf.build_mrbc_exec
  conf.enable_cxx_exception
  conf.cc.cxx_compile_flag = '-O3 -fno-ident -ffunction-sections -fdata-sections -fdelete-dead-exceptions -flto -ffast-math -ffp-contract=fast'
  conf.disable_compile_commands
  conf.disable_file_prefix_map
  conf.defines << 'MRB_MAIN_PROFILE'
  conf.defines << 'MRB_UTF8_STRING'
  conf.gem :core => 'mruby-array-ext'
  conf.gem :core => 'mruby-bin-mrbc'
  conf.gem :core => 'mruby-bin-mruby'
  conf.gem :core => 'mruby-binding'
  conf.gem :core => 'mruby-catch'
  conf.gem :core => 'mruby-class-ext'
  conf.gem :core => 'mruby-compiler'
  conf.gem :core => 'mruby-enum-chain'
  conf.gem :core => 'mruby-enum-ext'
  conf.gem :core => 'mruby-enum-lazy'
  conf.gem :core => 'mruby-enumerator'
  conf.gem :core => 'mruby-errno'
  conf.gem :core => 'mruby-error'
  conf.gem :core => 'mruby-eval'
  conf.gem :core => 'mruby-fiber'
  conf.gem :core => 'mruby-hash-ext'
  conf.gem :core => 'mruby-kernel-ext'
  conf.gem :core => 'mruby-math'
  conf.gem :core => 'mruby-method'
  conf.gem :core => 'mruby-numeric-ext'
  conf.gem :core => 'mruby-object-ext'
  conf.gem :core => 'mruby-proc-ext'
  conf.gem :core => 'mruby-random'
  conf.gem :core => 'mruby-regexp'
  conf.gem :core => 'mruby-string-ext'
  conf.gem :core => 'mruby-symbol-ext'
  conf.gem :core => 'mruby-toplevel-ext'
end
