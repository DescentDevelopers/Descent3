set(HEADERS)
set(CPPS
  networking.cpp)

set(PLATFORMCPPS)

if(WIN32)
  set(PLATFORMCPPS "directplay.cpp")
endif()

add_library(networking STATIC ${HEADERS} ${CPPS} ${PLATFORMCPPS})
target_link_libraries(networking PRIVATE
  ddio_common
)
