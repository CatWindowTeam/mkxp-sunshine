#pragma once
#include <SDL3/SDL_stdinc.h>
struct RenderStats{
	Uint32 drawCalls;
	Uint32 vertices;
	Uint32 uploads;
	Uint64 uploadBytes;
};

extern RenderStats renderStats;

inline void renderStatsUpload(size_t bytes){
	++renderStats.uploads;
	renderStats.uploadBytes += bytes;
}

void renderStatsBeginSwap();
void renderStatsEndSwap();
