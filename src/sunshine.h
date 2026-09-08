#pragma once
#include <boost/chrono.hpp>

class Bitmap;

//помойка ебаная
inline boost::chrono::high_resolution_clock::time_point startTime;

class Sunshine {
public:
	Sunshine();
	~Sunshine();

	void loadNoise();
	
	// TODO: find a better place for stuff like that, prob make some .cpp for OS stuff.
	enum DisplayServerType : int {
		Unknown = 0,
		X11,
		Wayland,
		Cocoa
	};
	
	DisplayServerType displayServerType() const;
	
	Bitmap* noiseBitmap() const;

private:
	Bitmap* noise;
};
