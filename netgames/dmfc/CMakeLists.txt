set(HEADERS
  dmfcdllinit.h
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

add_definitions(-DOUTRAGE_VERSION -DDMFC_DLL)

add_library(dmfc SHARED ${HEADERS} ${CPPS})

add_custom_command(TARGET dmfc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dmfc> "$<TARGET_FILE_DIR:Descent3>"
    COMMENT "Generate dmfc")