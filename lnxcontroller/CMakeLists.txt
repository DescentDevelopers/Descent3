set(HEADERS)
set(CPPS lnxcontroller.cpp)

add_library(lnxcontroller STATIC ${HEADERS} ${CPPS})
target_link_libraries(lnxcontroller PRIVATE
  cfile
)
