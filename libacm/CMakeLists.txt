SET (HEADERS )
SET (CPPS
	aencode.cpp
	libacm.cpp)

ADD_LIBRARY(libacm STATIC ${HEADERS} ${CPPS})