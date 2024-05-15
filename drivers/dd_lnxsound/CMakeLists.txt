set(HEADERS ddlnxsound.h)
set(CPPS mixer.cpp sdlsound.cpp ../dd_sndlib/ssl_lib.cpp ../dd_sndlib/ddsoundload.cpp)

add_library(dd_lnxsound STATIC ${HEADERS} ${CPPS})
