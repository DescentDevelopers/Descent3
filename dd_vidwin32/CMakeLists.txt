set(HEADERS 
		ddvidlib.h)
set(CPPS
		video_win32.cpp
		vidWin32FS.cpp
		vidWin32Win.cpp)

add_library(dd_vidwin32 STATIC ${HEADERS} ${CPPS})