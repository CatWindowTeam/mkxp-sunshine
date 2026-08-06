set(CUSTOM_FLAGS
  -minline-float-divide-min-latency
  -minline-float-divide-max-throughput
  -minline-int-divide-min-latency
  -minline-int-divide-max-throughput
  -minline-sqrt-min-latency
  -minline-sqrt-max-throughput
)

string(JOIN " " CUSTOM_FLAGS_STR ${CUSTOM_FLAGS})
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${CUSTOM_FLAGS_STR}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CUSTOM_FLAGS_STR}")
