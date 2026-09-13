#pragma once
#include "sunshine.h"
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include "bitmap.h"
#include <string_view>

const char* NOISE_PATH = "Graphics/Misc/noise"; // TODO: find for this another place and make this not hardcoded right into engine.

Sunshine::Sunshine() : noise(nullptr) {}

void Sunshine::loadNoise() {
	if (this->noise == nullptr)
		return;

	this->noise = new Bitmap(NOISE_PATH);
}

Bitmap* Sunshine::noiseBitmap() const {
	return this->noise;
}

Sunshine::~Sunshine() {
	if (Sunshine::noise != nullptr)
		delete Sunshine::noise;
}
