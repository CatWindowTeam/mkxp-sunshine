#include "renderstats.h"
#include "config.h"
#include "debugwriter.h"
#include "render/irender.h"
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_stdinc.h>

RenderStats renderStats;

static Uint64 lastSwapEnd = 0;
static Uint64 swapStart = 0;
static Uint64 workNs = 0;
static Uint64 swapNs = 0;
static Uint64 windowStart = 0;
static Uint32 frames = 0;
static Uint64 totalDraws = 0;
static Uint64 totalVertices = 0;
static Uint64 totalUploads = 0;
static Uint64 totalUploadBytes = 0;

void renderStatsBeginSwap(){
	if(!conf.renderStats)
		return;

	swapStart = SDL_GetTicksNS();
	if(lastSwapEnd)
		workNs += swapStart - lastSwapEnd;
}

void renderStatsEndSwap(){
	if (!conf.renderStats)
		return;

	const uint64_t now = SDL_GetTicksNS();
	swapNs += now - swapStart;
	lastSwapEnd = now;

	++frames;
	totalDraws += renderStats.drawCalls;
	totalVertices += renderStats.vertices;
	totalUploads += renderStats.uploads;
	totalUploadBytes += renderStats.uploadBytes;
	renderStats = RenderStats();

	if(!windowStart)
		windowStart = now;

	if(now - windowStart < 1000000000ull)
		return;

	const double f = frames;
	char line[256];
	SDL_snprintf(line, sizeof(line), "[RS] fps %d | draws %d | verts %d | uploads %.1f (%d KB) | cpu %.2f ms | swap %.2f ms",
	         (int) (f * 1e9 / (now - windowStart)),
	         (int) (totalDraws / f), (int) (totalVertices / f),
	         totalUploads / f, (int) (totalUploadBytes / f / 1024),
	         workNs / f / 1e6, swapNs / f / 1e6);
	Debug() << line;

	windowStart = now;
	frames = 0;
	workNs = swapNs = 0;
	totalDraws = totalVertices = totalUploads = totalUploadBytes = 0;
}
