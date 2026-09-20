#pragma once
#include <chrono>

class Bitmap;
inline std::chrono::high_resolution_clock::time_point startTime;

class Sunshine {
public:
	Sunshine();
	~Sunshine();
	void loadNoise();
	Bitmap* noiseBitmap() const;

private:
	Bitmap* noise;
};
