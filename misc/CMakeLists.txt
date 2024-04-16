SET (HEADERS )
SET (CPPS
		endian.cpp
		error.cpp
		logfile.cpp
		psglob.cpp
		psrand.cpp
		pstring.cpp)

ADD_LIBRARY(misc STATIC ${HEADERS} ${CPPS})