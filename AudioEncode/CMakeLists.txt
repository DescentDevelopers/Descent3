set(HEADERS
  ../lib/Aencode.h
  ../lib/audio_encode.h
)
set(CPPS
  encoder.cpp)

add_library(AudioEncode STATIC ${HEADERS} ${CPPS})
target_link_libraries(AudioEncode libacm)
