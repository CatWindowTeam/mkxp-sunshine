#include "sunshine.h"
#include "bitmap.h"

const char* NOISE_PATH = "Graphics/Misc/noise";
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
