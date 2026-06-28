#include <boost/chrono.hpp>

#include "bitmap.h"
//помойка ебаная
inline boost::chrono::high_resolution_clock::time_point startTime;

#if defined(__linux__) || defined(BSD) || defined(__sun) || defined(__APPLE__) || \
    defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || \
    defined(__GNU__) || defined(__hurd__) || defined(__DragonFly__)
  #define unix_like 1
#endif


class Sunshine{
public:
    Sunshine();

    const char* noisePath = "Graphics/Misc/noise";

    Bitmap* noiseBitmap;

    void loadNoise();

private:
    bool noiseLoaded = false;
};
