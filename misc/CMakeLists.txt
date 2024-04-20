set(HEADERS )
set(CPPS
		endian.cpp
		error.cpp
		logfile.cpp
		psglob.cpp
		psrand.cpp
		pstring.cpp)

add_library(misc STATIC ${HEADERS} ${CPPS})