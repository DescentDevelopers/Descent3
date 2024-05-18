set(HEADERS registry.h)
set(CPPS
  lnxcon.cpp
  lnxcon_raw.cpp
  lnxapp.cpp
  lnxcon_null.cpp
  lnxcontroller.cpp
  lnxdata.cpp
  registry.cpp)
add_library(linux STATIC ${HEADERS} ${CPPS})
target_link_libraries(linux PRIVATE
  cfile
)
