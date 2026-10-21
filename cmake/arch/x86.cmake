set(ARCH_COMMON -march=generic32 -mtune=generic -m32)
set(ARCH_RELEASE
	-mnoreturn-no-callee-saved-registers
	-mindirect-branch=keep
	-mfunction-return=keep
	-mharden-sls=none
	-mno-shstk
	-fcf-protection=none
	-mno-record-return
	-mno-retpoline
	-mno-ibt)
set(ARCH_DEBUG "")
