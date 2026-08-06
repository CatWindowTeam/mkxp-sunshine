set(CUSTOM_FLAGS
  -mnoreturn-no-callee-saved-registers
  -mrelax-cmpxchg-loop
)

string(JOIN " " CUSTOM_FLAGS_STR ${CUSTOM_FLAGS})
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${CUSTOM_FLAGS_STR}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CUSTOM_FLAGS_STR}")
