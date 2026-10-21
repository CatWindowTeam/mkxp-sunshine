set(ARCH_COMMON "")
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
