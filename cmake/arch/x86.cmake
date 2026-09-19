set(ARCH_COMMON -march=x86-64 -mtune=generic)
set(ARCH_RELEASE
	-mnoreturn-no-callee-saved-registers
	-mno-return-callee-saved-registers
	-momit-leaf-frame-pointer
	-mindirect-branch=keep
	-mfunction-return=keep
	-mharden-sls=none
	-mno-shstk
	-fcf-protection=none
	-mno-record-return)
set(ARCH_DEBUG "")
