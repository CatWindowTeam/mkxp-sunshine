#pragma once
#include "sunshine.h"

Sunshine::Sunshine() {}

void Sunshine::loadNoise(){
    if(!noiseLoaded){
        Sunshine::noiseBitmap = new Bitmap(Sunshine::noisePath);
        noiseLoaded = true;
    }
}
