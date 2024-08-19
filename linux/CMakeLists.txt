set(HEADERS
  lnxapp.h
  lnxcontroller.h
  registry.h)
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
  plog::plog
)
target_include_directories(linux PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/linux
  >
)
target_link_libraries(linux PRIVATE
  ddio
  misc
  SDL2::SDL2
)
