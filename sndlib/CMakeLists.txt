set(HEADERS
  auddev.h
  ddsndgeometry.h
  ds3dlib.h
  ds3dlib_internal.h
  hlsoundlib.h
  mixer.h
  sdlsound.h
  sndrender.h
  soundload.h
  ssl_lib.h
  vmanpset.h)
set(CPPS
  hlsoundlib.cpp
  sndrender.cpp
  soundload.cpp
  ddsoundload.cpp
  ssl_lib.cpp
  mixer.cpp
  sdlgeometry.cpp
  sdlsound.cpp
)

add_library(sndlib STATIC ${HEADERS} ${CPPS})
target_link_libraries(sndlib PRIVATE
  cfile
  ddio
  mem
  misc
  stream_audio
  physics
  SDL2::SDL2
)
target_include_directories(sndlib PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/sndlib
  >
)
