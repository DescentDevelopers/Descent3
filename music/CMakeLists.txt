set(HEADERS
  music.h
  musiclib.h)
set(CPPS
  omflex.cpp
  sequencer.cpp
  streamer.cpp
  tracklist.cpp)

add_library(music STATIC ${HEADERS} ${CPPS})
target_link_libraries(music
  AudioEncode
  cfile
  ddebug
  ddio
  logger
  mem
  sndlib
  stream_audio
)
target_include_directories(music PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/music
  >
)
