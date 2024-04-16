SET (HEADERS ddio_lnx.h )
SET (CPPS lnxfile.cpp lnxforcefeedback.cpp lnxio.cpp sdljoy.cpp lnxkey.cpp lnxkey_null.cpp lnxmouse.cpp lnxtimer.cpp lnxkey_raw.cpp lnxcdrom.cpp lnxkey_sdl.cpp)

ADD_LIBRARY(ddio_lnx STATIC ${HEADERS} ${CPPS})