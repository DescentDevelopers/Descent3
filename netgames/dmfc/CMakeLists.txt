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

add_library(dmfc STATIC ${HEADERS} ${CPPS})

target_include_directories(dmfc PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/grtext
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/module
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/renderer
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/bitmap
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/networking
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/physics
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/model
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/manage
  >
)
