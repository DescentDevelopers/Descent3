SET (HEADERS )
SET (CPPS
		CFILE.cpp
		hog.cpp
		InfFile.cpp)

ADD_LIBRARY(cfile STATIC ${HEADERS} ${CPPS})