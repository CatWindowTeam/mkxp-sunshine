target_compile_options(${APP_TARGET} PRIVATE
	${COPT_C} ${COPT_CXX}
	${COPT_UNSAFE}
	${COPT_UNSAFE_CXX}
	${COPT_NATIVE} ${COPT_LTO}
)
target_link_options(${APP_TARGET} PRIVATE ${LOPT_BASE})
