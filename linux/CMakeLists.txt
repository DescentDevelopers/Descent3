set(CPPS
  lnxcon.cpp
  lnxcon_raw.cpp
  lnxcontroller.cpp
  lnxapp.cpp
  lnxcon_null.cpp
  lnxdata.cpp
  registry.cpp
)

add_library(linux STATIC ${CPPS})
target_link_libraries(linux PRIVATE
  cfile
)
