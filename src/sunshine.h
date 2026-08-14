#include <boost/chrono.hpp>

#include "bitmap.h"
//помойка ебаная
inline boost::chrono::high_resolution_clock::time_point startTime;
class Sunshine{
public:
    Sunshine();

    const char* noisePath = "Graphics/Misc/noise";

    Bitmap* noiseBitmap;

    void loadNoise();

private:
    bool noiseLoaded = false;
};
