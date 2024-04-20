set(HEADERS )
set(CPPS
	aencode.cpp
	libacm.cpp)

add_library(libacm STATIC ${HEADERS} ${CPPS})