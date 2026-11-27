// Functions for mobile or portable devices support
#include "portable.h"
#include "define.h"
#include <physfs.h>

void mount_assets_archive(){
	#ifdef vita
		if(!PHYSFS_mount("app0:assets.zip", nullptr, 0)) {
		    WarnMsg("Failed to mount assets archive: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
	#elif mkxp_android
	        if(!PHYSFS_mount("assets://assets.zip", nullptr, 0)) {
                    WarnMsg("Failed to mount assets archive: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
                }
	#endif
}
