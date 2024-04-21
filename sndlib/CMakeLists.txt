set(HEADERS sndrender.h)
set(CPPS
  hlsoundlib.cpp
  sndrender.cpp
  soundload.cpp)

add_library(sndlib STATIC ${HEADERS} ${CPPS})
