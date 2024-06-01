set(HEADERS
  linux_fix.h
  lnxapp.h
  lnxcontroller.h
  registry.h
)

set(CPPS
  lnxcon.cpp
  lnxcon_raw.cpp
  lnxcontroller.cpp
  lnxapp.cpp
  lnxcon_null.cpp
  lnxdata.cpp
  registry.cpp
)

add_library(linux STATIC ${HEADERS} ${CPPS})
target_link_libraries(linux PRIVATE
  cfile
)
target_include_directories(linux PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/networking
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/renderer
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/model
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/manage
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/Descent3
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/2dlib
  >
)
target_link_libraries(linux PRIVATE
  ddio
)
