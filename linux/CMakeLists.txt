SET (HEADERS registry.h )
SET (CPPS
  lnxcon.cpp
  lnxcon_raw.cpp
  lnxdebug.cpp
  lnxapp.cpp
  lnxcon_null.cpp
  lnxdata.cpp
  lnxmono.cpp
  registry.cpp)
#  DynXLib.cpp
ADD_LIBRARY(linux STATIC ${HEADERS} ${CPPS})