SET (HEADERS sndrender.h )
SET (CPPS
		hlsoundlib.cpp
		sndrender.cpp
		soundload.cpp)

ADD_LIBRARY(sndlib STATIC ${HEADERS} ${CPPS})