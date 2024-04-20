set(HEADERS )
set(CPPS
		osfarchive.cpp
		streamaudio.cpp)

add_library(stream_audio STATIC ${HEADERS} ${CPPS})
target_link_libraries(stream_audio libacm)