#pragma once
#include "sunshine.h"
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

#include <string_view>

#ifndef IS_WINDOWS
#ifdef _WIN32
#define IS_WINDOWS true
#else
#define IS_WINDOWS false
#endif
#endif

Sunshine::Sunshine() {}

void Sunshine::loadNoise(){
	if(!noiseLoaded){
		Sunshine::noiseBitmap = new Bitmap(Sunshine::noisePath);
		noiseLoaded = true;
	}
}

// TODO: find a better place for stuff like that, prob make some .cpp for OS stuff.
Sunshine::DisplayServerType Sunshine::displayServerType() const {
	if (IS_WINDOWS) // compilers are not dumb. this shit is for readability.
		return DisplayServerType::Unknown;

	std::string_view name = SDL_GetCurrentVideoDriver();
	{
		if (name == "x11")     return DisplayServerType::X11;
		if (name == "wayland") return DisplayServerType::Wayland;
		if (name == "cocoa")   return DisplayServerType::Cocoa;
	}
	return DisplayServerType::Unknown;
}