#pragma once
#include "exception.h"
#include <SDL3/SDL_video.h>
#include <vector>
#include <string>

inline std::vector<std::string> logs = {};

inline bool show_crash_sceen = false;
inline bool is_ruby_initialized = false;
inline char crash_message[1024] = "";
inline const char* crash_reason = "Unknown";
inline const char* crash_possible_solution = "Unknown";
void crash(Exception::Type t, const char *fmt, ...);
void ErrorMsg(const char *fmt, ...);
void ErrorMsg(Exception::Type t, const char *fmt, ...);
void WarnMsg(const char *fmt, ...);
void crash_screen(SDL_Window* win);
