set(HEADERS
  dmfcinputcommands.h
  dmfcinternal.h
  encryption.h)

set(CPPS
  dmfcbase.cpp
  dmfccfg.cpp
  dmfcclient.cpp
  dmfcfunctions.cpp
  dmfchudmessages.cpp
  dmfcinputcommand.cpp
  dmfcmenu.cpp
  dmfcpackets.cpp
  dmfcpinfo.cpp
  dmfcprecord.cpp
  dmfcremote.cpp
  dmfcserver.cpp
  dmfcstats.cpp
  dmfctimer.cpp
  dmfcui.cpp
  dmfcvirtual.cpp
  idmfc.cpp
  encryption.cpp
  dmfcinterface.cpp)

add_definitions(-DOUTRAGE_VERSION)

add_library(dmfc STATIC ${HEADERS} ${CPPS})
target_link_libraries(dmfc PUBLIC
  SDL3::SDL3
  grtext
  logger
  misc
  module
  physics
  ddio
)
target_include_directories(dmfc PRIVATE ${SDL3_INCLUDE_DIRS})