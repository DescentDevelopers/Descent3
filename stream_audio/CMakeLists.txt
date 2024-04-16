SET (HEADERS )
SET (CPPS
		osfarchive.cpp
		streamaudio.cpp)

ADD_LIBRARY(stream_audio STATIC ${HEADERS} ${CPPS})
target_link_libraries(stream_audio libacm)