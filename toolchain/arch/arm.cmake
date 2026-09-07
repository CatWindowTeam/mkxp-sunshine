set(ARCH_COMMON "")
set(ARCH_RELEASE
	-momit-leaf-frame-pointer
	-mlow-precision-recip-sqrt
	-mlow-precision-sqrt
	-mlow-precision-div
	-mearly-ra=all
	-mbranch-protection=none)
set(ARCH_DEBUG "")
