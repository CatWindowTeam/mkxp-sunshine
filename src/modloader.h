#include <string>
#include <set>
void ModLoader();

inline bool modloader_is_enabled = false;
inline unsigned int mods_count = 0;
inline unsigned int preloaded_script_count = 0;
inline std::set<std::string> preloadScripts;
