#pragma once
#include "sunshine.h"
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

#include "bitmap.h"

#include <string_view>

const char* NOISE_PATH = "Graphics/Misc/noise"; // TODO: find for this another place and make this not hardcoded right into engine.

#ifndef IS_WINDOWS
#ifdef _WIN32
#define IS_WINDOWS true
#else
#define IS_WINDOWS false
#endif
#endif

Sunshine::Sunshine() :
	noise(nullptr)
{}

void Sunshine::loadNoise() {
	if (this->noise == nullptr)
		return;

	this->noise = new Bitmap(NOISE_PATH);
}

Bitmap* Sunshine::noiseBitmap() const {
	return this->noise;
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

Sunshine::~Sunshine() {
	if (Sunshine::noise != nullptr)
		delete Sunshine::noise;
}