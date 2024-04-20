set(HEADERS registry.h)
set(CPPS
  lnxcon.cpp
  lnxcon_raw.cpp
  lnxapp.cpp
  lnxcon_null.cpp
  lnxdata.cpp
  registry.cpp)
#  DynXLib.cpp
add_library(linux STATIC ${HEADERS} ${CPPS})
