SET (HEADERS ddlnxsound.h )
SET (CPPS mixer.cpp sdlsound.cpp ../dd_sndlib/ssl_lib.cpp ../dd_sndlib/ddsoundload.cpp)

ADD_LIBRARY(dd_lnxsound STATIC ${HEADERS} ${CPPS})