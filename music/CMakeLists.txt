SET (HEADERS )
SET (CPPS
		omflex.cpp
		sequencer.cpp
		streamer.cpp
		tracklist.cpp)

ADD_LIBRARY(music STATIC ${HEADERS} ${CPPS})