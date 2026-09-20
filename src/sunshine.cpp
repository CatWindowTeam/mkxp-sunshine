#include <SDL3/SDL_iostream.h>

#include "sunshine.h"
#include "bitmap.h"
#include "noise.png.xxd"

Sunshine::Sunshine() : noise(nullptr) {}

void Sunshine::loadNoise() {
	if (this->noise != nullptr)
		return;

	SDL_IOStream* rawNoise = SDL_IOFromConstMem(assets_noise_png, assets_noise_png_len);

	this->noise = new Bitmap(rawNoise);

	SDL_CloseIO(rawNoise);
}

Bitmap* Sunshine::noiseBitmap() const {
	return this->noise;
}

Sunshine::~Sunshine() {
	if (Sunshine::noise != nullptr)
		delete Sunshine::noise;
}
