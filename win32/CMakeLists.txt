set(HEADERS win32os.h )
set(CPPS
		winapp.cpp
		wincon.cpp
		WinController.cpp
		windata.cpp
		windebug.cpp
		winmono.cpp
		wintask.cpp)

add_library(win32 STATIC ${HEADERS} ${CPPS})