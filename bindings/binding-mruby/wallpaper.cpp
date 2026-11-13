#include <SDL3/SDL_platform_defines.h>
#include <mruby.h>

#if SDL_PLATFORM_WINDOWS
	#include "wallpaper/w_windows.h"
#elif SDL_PLATFORM_UNIX
	#include "wallpaper/w_unix.h"
#else
	#include "wallpaper/w_dummy.h"
#endif

void wallpaperBindingInit(mrb_state *mrb){
	auto module = mrb_define_module(mrb, "Wallpaper");

	// Functions
	mrb_define_module_function(mrb, module, "set", wallpaperSet, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module, "reset", wallpaperReset, MRB_ARGS_NONE());
}
