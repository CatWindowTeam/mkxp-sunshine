#pragma once
#include "exception.h"
#include <SDL3/SDL_video.h>
#include <vector>
#include <string>
inline std::vector<std::string> logs = {};
inline std::vector<std::string> modloader_logs = {};

// 4 - debug
// 3 - info
// 2 - warn
// 1 - error
// 0 - fatal error 
inline int loglevel = 3;

inline bool show_crash_screen = false;
inline bool is_privacy_crashdump_enabled = false;
inline bool is_ruby_initialized = false;
inline char crash_message[1024] = "";
void crash(Exception::Type t, const char *fmt, ...);
void ErrorMsg(const char *fmt, ...);
void ErrorMsg(Exception::Type t, const char *fmt, ...);
void WarnMsg(const char *fmt, ...);
void crash_screen(SDL_Window* win);
void terminate_stacktrace();
