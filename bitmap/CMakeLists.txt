SET (HEADERS iff.h)
SET (CPPS
		bitmain.cpp
		bumpmap.cpp
		iff.cpp
		lightmap.cpp
		pcx.cpp
		tga.cpp)

ADD_LIBRARY(bitmap STATIC ${HEADERS} ${CPPS})