set(HEADERS)
set(CPPS
		omflex.cpp
		sequencer.cpp
		streamer.cpp
		tracklist.cpp)

add_library(music STATIC ${HEADERS} ${CPPS})