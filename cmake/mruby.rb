MRuby::Build.new("host") do |conf|
  conf.toolchain :gcc
  conf.cc.command       = "gcc"
  conf.cxx.command      = "g++"
  conf.linker.command   = "g++"
  conf.archiver.command = "ar"
end

MRuby::CrossBuild.new("sunshine")  do |conf|
  conf.toolchain

  if(ENV["PLATFORM"] == "DOS")
  	# If DJGPP is not in the PATH, set this to the bin directory
  	DJGPP_PATH = nil

  	GCC = 'i586-pc-msdosdjgpp-gcc'
  	GXX = 'i586-pc-msdosdjgpp-g++'
  	AR  = 'i586-pc-msdosdjgpp-ar'

  	conf.cc do |cc|
  	  cc.command = DJGPP_PATH ? File.join(DJGPP_PATH, GCC) : GCC
  	  cc.defines << 'MRB_NO_IO_PREAD_PWRITE'
  	end

  	conf.cxx do |cxx|
  	  cxx.command = DJGPP_PATH ? File.join(DJGPP_PATH, GXX) : GXX
  	  cxx.defines << 'MRB_NO_IO_PREAD_PWRITE'
  	end

  	conf.linker do |linker|
  	  linker.command = DJGPP_PATH ? File.join(DJGPP_PATH, GXX) : GXX
  	  linker.libraries = %w(m)
  	end

  	conf.archiver do |archiver|
  	  archiver.command = DJGPP_PATH ? File.join(DJGPP_PATH, AR) : AR
  	end
  end
  
  conf.bins = []
  conf.enable_cxx_exception
  conf.cc.cxx_compile_flag = ENV["MRUBY_BUILD_FLAGS"]
  conf.disable_compile_commands
  conf.disable_file_prefix_map
  conf.defines << 'MRB_MAIN_PROFILE'
  conf.defines << 'MRB_UTF8_STRING'
  if(ENV["LOW_MEMORY"] == "ON")
	conf.defines << 'MRB_GC_STRESS'
  end
  conf.gem :core => 'mruby-array-ext'
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
