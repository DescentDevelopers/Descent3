SET (HEADERS lib2d.h)
SET (CPPS
	font.cpp
	hardsurf.cpp
	memsurf.cpp
	pen.cpp
	pentext.cpp
	screen.cpp
	surface.cpp
	viewport.cpp)

ADD_LIBRARY(2dlib STATIC ${HEADERS} ${CPPS})
    