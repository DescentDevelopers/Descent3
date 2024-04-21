set(HEADERS )
set(CPPS
	aencode.cpp
	#libacm.cpp
	decode.c
	)

add_library(libacm STATIC ${HEADERS} ${CPPS})