SET (HEADERS )
SET (CPPS
		encoder.cpp)

ADD_LIBRARY(AudioEncode STATIC ${HEADERS} ${CPPS})
target_link_libraries(AudioEncode libacm)