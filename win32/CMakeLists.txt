SET (HEADERS win32os.h )
SET (CPPS
		winapp.cpp
		wincon.cpp
		WinController.cpp
		windata.cpp
		windebug.cpp
		winmono.cpp
		wintask.cpp)

ADD_LIBRARY(win32 STATIC ${HEADERS} ${CPPS})