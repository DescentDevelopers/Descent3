set(HEADERS
  lnxapp.h
  registry.h)
set(CPPS
  lnxcon.cpp
  lnxcon_raw.cpp
  lnxapp.cpp
  lnxcon_null.cpp
  lnxdata.cpp
  registry.cpp
)

add_library(linux STATIC ${HEADERS} ${CPPS})
target_link_libraries(linux PRIVATE
  cfile
  logger
)
target_include_directories(linux PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/linux
  >
)
target_link_libraries(linux PRIVATE
  ddebug
  ddio
  SDL2::SDL2
)
