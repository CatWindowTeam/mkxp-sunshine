set(ARCH_COMMON "")
set(ARCH_RELEASE
	-mnoreturn-no-callee-saved-registers
	-mrelax-cmpxchg-loop
	-momit-leaf-frame-pointer
	-mavoid-false-dependencies)
set(ARCH_DEBUG "")
