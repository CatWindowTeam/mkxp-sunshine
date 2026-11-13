#include "define.h"
#include <SDL3/SDL_platform_defines.h>
#include <ruby.h>
#if windows
	#include "wallpaper/w_windows.h"
#elif SDL_PLATFORM_HURD
	//TODO: fix gio on Hurd
	#include "wallpaper/w_dummy.h"
#elif TERMUX
	#include "wallpaper/w_dummy.h"
#elif unix_like
	#include "wallpaper/w_unix.h"
#else
	#include "wallpaper/w_dummy.h"
#endif

void wallpaperBindingInit(){
	VALUE module = rb_define_module("Wallpaper");

	// Functions
	rb_define_module_function(module, "set", RUBY_METHOD_FUNC(wallpaperSet), -1);
	rb_define_module_function(module, "reset", RUBY_METHOD_FUNC(wallpaperReset), 0);
}
