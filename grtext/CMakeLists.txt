SET (HEADERS grtextlib.h )
SET (CPPS
		grfont.cpp
		grtext.cpp
		textaux.cpp)

ADD_LIBRARY(grtext STATIC ${HEADERS} ${CPPS})