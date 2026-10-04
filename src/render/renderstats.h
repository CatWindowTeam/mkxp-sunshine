#pragma once
#include <stddef.h>
#include <stdint.h>

struct RenderStats{
	uint32_t drawCalls;
	uint32_t vertices;
	uint32_t uploads;
	uint64_t uploadBytes;
};

extern RenderStats renderStats;

inline void renderStatsUpload(size_t bytes){
	++renderStats.uploads;
	renderStats.uploadBytes += bytes;
}

void renderStatsBeginSwap();
void renderStatsEndSwap();
