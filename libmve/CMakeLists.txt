SET (HEADERS mvegfx.h
		mvelibi.h
		mvelibl.h
		platform.h
		snd8to16.h
		SystemInterfaces.h )
SET (CPPS
		mveasm.cpp
		mvelibl.cpp
		platform.cpp)



SET (PLATFORM_CPPS )

IF (UNIX)
  SET (PLATFORM_CPPS "lnxdsound.cpp")
ENDIF()

ADD_LIBRARY(libmve STATIC ${HEADERS} ${CPPS} ${PLATFORM_CPPS})

