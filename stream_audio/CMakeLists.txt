set(HEADERS
  streamaudio.h)
set(CPPS
  osfarchive.cpp
  streamaudio.cpp
)

add_library(stream_audio STATIC ${HEADERS} ${CPPS})
target_link_libraries(stream_audio PRIVATE
  ddio
  mem
  misc
  sndlib
)
target_link_libraries(stream_audio PUBLIC
  AudioEncode
)
target_include_directories(stream_audio PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/stream_audio
  >
)
