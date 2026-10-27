// Functions for mobile or portable devices support
#include "portable.h"
#include "define.h"
#include <physfs.h>
#include <SDL3/SDL_stdinc.h>

void mount_assets_archive(){
	#ifdef vita
		if(!PHYSFS_mount("app0:sunshine.zip", nullptr, 0)) {
		    WarnMsg("Failed to mount assets archive: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
	#elif mkxp_android
		char[256] path = "";
		SDL_snprintf(path, sizeof(path), "%s/sunshine.zip", PHYSFS_getBaseDir());
		if(!PHYSFS_mount("app0:sunshine.zip", nullptr, 0)){
		    WarnMsg("Failed to mount assets archive: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
	#endif	
}
