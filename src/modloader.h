#include <string>
#include <set>
#include <SDL3/SDL_video.h>
#include "config.h"
void ModLoader(Config conf, SDL_Window* win);

inline bool modloader_is_enabled = false;
inline unsigned int mods_count = 0;
inline unsigned int preloaded_script_count = 0;
inline std::set<std::string> preloadScripts;
