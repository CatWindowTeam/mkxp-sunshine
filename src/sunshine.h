#include <boost/chrono.hpp>

#include "bitmap.h"
//помойка ебаная
inline boost::chrono::high_resolution_clock::time_point startTime;

extern bool is_privacy_crashdump_enabled;
class Sunshine{
public:
    Sunshine();

    const char* noisePath = "Graphics/Misc/noise";

    Bitmap* noiseBitmap;

    void loadNoise();

private:
    bool noiseLoaded = false;
};
