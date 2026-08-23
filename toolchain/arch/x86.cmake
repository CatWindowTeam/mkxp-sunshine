set(ARCH_COMMON "")
set(ARCH_RELEASE
	-mnoreturn-no-callee-saved-registers
	-mrelax-cmpxchg-loop
	-momit-leaf-frame-pointer
	-mgather
	-mscatter
	-mcldemote
	-mbranches-within-32B-boundaries
	-mavoid-false-dependencies)
set(ARCH_DEBUG "")
