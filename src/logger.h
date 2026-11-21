#pragma once
#include <iostream>
#include <SDL3/SDL_stdinc.h>
#ifdef SUNSHINE_LOG_COLORED
	#include "rang.hpp"
#endif

void LogC(const char *fmt, ...);
void LogCXX(Args&&... args);
