SET (HEADERS 
		ddgrWin32.h
		ddgrWin32DX.h
		ddgrWin32GDI.h)
SET (CPPS
		ddgrWin32API.cpp
		ddgrWin32DX.cpp
		ddgrWin32GDI.cpp
		ddgrWin32Init.cpp)

ADD_LIBRARY(dd_grwin32 STATIC ${HEADERS} ${CPPS})