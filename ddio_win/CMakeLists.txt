SET (HEADERS ddio_win.h )
SET (CPPS
		serial.cpp
		winfile.cpp
		winforcefeedback.cpp
		winio.cpp
		winjoy.cpp
		winkey.cpp
		winmouse.cpp
		wintimer.cpp)

ADD_LIBRARY(ddio_win STATIC ${HEADERS} ${CPPS})