set(ARCH_COMMON -march=x86-64 -mtune=generic)
set(ARCH_RELEASE
	-mnoreturn-no-callee-saved-registers
	-mrelax-cmpxchg-loop
	-momit-leaf-frame-pointer
	-mavoid-false-dependencies
	-fno-cf-protection)
set(ARCH_DEBUG "")
