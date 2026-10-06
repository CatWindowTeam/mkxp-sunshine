#include "define.h"
#include <ruby.h>
#if windows
	#include "wallpaper/w_windows.h"
#elif unix_like
	#include "wallpaper/w_unix.h"
#else
	#include "wallpaper/dummy.h"
#endif

void wallpaperBindingInit(){
	VALUE module = rb_define_module("Wallpaper");

	// Functions
	rb_define_module_function(module, "set", RUBY_METHOD_FUNC(wallpaperSet), -1);
	rb_define_module_function(module, "reset", RUBY_METHOD_FUNC(wallpaperReset), 0);
}
