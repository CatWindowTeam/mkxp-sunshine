// https://github.com/moby/profiles/blob/main/seccomp/default.json
#include "security.h"
#include "meow.h"
#include "debugwriter.h"
#include <SDL3/SDL_system.h>
// this component is needed to protect users from mod attacks.

#ifdef __linux__
	#include <sys/socket.h>
	#include <seccomp.h>
	//Yes its not best way, anyway better than nothing.
	scmp_filter_ctx ctx;
	const int seccomplist[] = {SCMP_SYS(bpf),
	SCMP_SYS(set_mempolicy),
	SCMP_SYS(set_mempolicy_home_node),
	SCMP_SYS(vhangup),
	SCMP_SYS(settimeofday),
	SCMP_SYS(stime),
	SCMP_SYS(clock_settime), 
	SCMP_SYS(clock_settime64),
	SCMP_SYS(iopl),
	SCMP_SYS(ioperm),
	SCMP_SYS(ptrace),
	SCMP_SYS(process_vm_writev),
	SCMP_SYS(process_vm_readv),
	SCMP_SYS(process_madvise), 
	SCMP_SYS(pidfd_getfd),
	SCMP_SYS(kcmp),
	SCMP_SYS(delete_module),
	SCMP_SYS(init_module),
	SCMP_SYS(init_module),
	SCMP_SYS(chroot),
	SCMP_SYS(reboot),
	SCMP_SYS(unshare), 
	SCMP_SYS(umount2),
	SCMP_SYS(umount),
	SCMP_SYS(setns),
	SCMP_SYS(sethostname),
	SCMP_SYS(setdomainname),
	SCMP_SYS(bpf),
	SCMP_SYS(quotactl_fd),
	SCMP_SYS(quotactl), 
	SCMP_SYS(move_mount),
	SCMP_SYS(mount_setattr),
	SCMP_SYS(mount),
	SCMP_SYS(process_vm_readv),
	SCMP_SYS(process_vm_writev),
	SCMP_SYS(ptrace),
	SCMP_SYS(swapon),
	SCMP_SYS(swapoff),
	SCMP_SYS(settimeofday),
	SCMP_SYS(sethostname),
	SCMP_SYS(umount),
	SCMP_SYS(umount2),
	SCMP_SYS(vm86old),
	SCMP_SYS(vm86),
	SCMP_SYS(setgroups),
	SCMP_SYS(setgid),
	SCMP_SYS(setfsuid), 
	SCMP_SYS(setfsgid),
	SCMP_SYS(setdomainname),
	SCMP_SYS(setns),
	SCMP_SYS(setpgid),
	SCMP_SYS(pciconfig_write)};
#endif

void SecurityManagerInit(){
	#ifdef __linux__
		Debug() << "[SECURITY] initializing SECCOMP filter...";
		ctx = seccomp_init(SCMP_ACT_ALLOW); // Default action: Kill the process
		if(ctx == NULL) {
		    WarnMsg("Warning: Failed to load SECCOMP! If you receive this warning, IT IS NOT RECOMMENDED to add any third-party modifications to Sunshine!");
		}else{
			int n = sizeof(seccomplist) / sizeof(seccomplist[0]);
			for(int i = 0; i < n; ++i){
			    if(seccomp_rule_add(ctx, SCMP_ACT_KILL, seccomplist[i], 0) < 0) {
			        Debug() << "seccomp_rule_add failed for " << seccomplist[i];
			    }
			}

			//Extra rules
			//Deny all network connections
			if(seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(socket), 1, SCMP_CMP(0, SCMP_CMP_EQ, AF_INET))){
				Debug() << "seccomp_rule_add failed for extra rules (socket AF_INET)";
			}

			if(seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(socket), 1, SCMP_CMP(0, SCMP_CMP_EQ, AF_INET6))){
				Debug() << "seccomp_rule_add failed for extra rules (socket AF_INET6)";
			}

			if(seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(socketpair), 1, SCMP_CMP(0, SCMP_CMP_NE, AF_UNIX))){
				Debug() << "seccomp_rule_add failed for extra rules (socketpair)";
			}
			if(seccomp_load(ctx) < 0) {
			    WarnMsg("Warning: Failed to load SECCOMP! If you receive this warning, IT IS NOT RECOMMENDED to add any third-party modifications to Sunshine!");
			    seccomp_release(ctx);
			}
		}
		SDL_Sandbox Sandbox = SDL_GetSandbox();
		securitystate = "sandboxed_SECCOMP";
		if(Sandbox == SDL_SANDBOX_FLATPAK)
			securitystate = "sandboxed_FLATPAK";
		if(Sandbox == SDL_SANDBOX_SNAP)
			securitystate = "sandboxed_SNAP";
	#elif __APPLE__
    		if(SDL_GetSandbox() == SDL_SANDBOX_MACOS)
			securitystate = "sandboxed_MacOS";
	#else
		Debug() << "[SECURITY] SecurityManager doesn't support this platform.";
	#endif
}

void SecurityManagerDeInit(){
	#ifdef __linux__
		seccomp_release(ctx);
	#endif
}
