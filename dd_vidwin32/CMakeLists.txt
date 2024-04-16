SET (HEADERS 
		ddvidlib.h)
SET (CPPS
		video_win32.cpp
		vidWin32FS.cpp
		vidWin32Win.cpp)

ADD_LIBRARY(dd_vidwin32 STATIC ${HEADERS} ${CPPS})