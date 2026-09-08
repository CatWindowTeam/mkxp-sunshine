set(ARCH_COMMON -march=x86-64 -mtune=generic)
set(ARCH_RELEASE
	-mnoreturn-no-callee-saved-registers
	-mrelax-cmpxchg-loop
	-momit-leaf-frame-pointer
	-mavoid-false-dependencies
	-fno-cf-protection
	-mindirect-branch=keep
	-mfunction-return=keep
	-mharden-sls=none
	-mno-shstk)
set(ARCH_DEBUG "")
