set(HEADERS)
set(CPPS
  encoder.cpp)

add_library(AudioEncode STATIC ${HEADERS} ${CPPS})
target_link_libraries(AudioEncode libacm)
