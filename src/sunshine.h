#pragma once
#include <boost/chrono.hpp>

class Bitmap;
	inline boost::chrono::high_resolution_clock::time_point startTime;

class Sunshine {
public:
	Sunshine();
	~Sunshine();
	void loadNoise();
	Bitmap* noiseBitmap() const;

private:
	Bitmap* noise;
};
