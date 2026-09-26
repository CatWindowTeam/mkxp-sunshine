set(ARCH_COMMON "")
set(ARCH_RELEASE
	-momit-leaf-frame-pointer
	-mearly-ra=all
	-mbranch-protection=none
	-mno-tpcs-frame
	-mno-apcs-leaf-frame
	-msign-return-address=none)
set(ARCH_DEBUG "")
