set(HEADERS )
set(CPPS
	aencode.cpp
	decode.c
	)
add_library(libacm STATIC ${HEADERS} ${CPPS})