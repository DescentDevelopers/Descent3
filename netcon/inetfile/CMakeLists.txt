set(HEADERS
  CFtp.h
  Chttpget.h
  inetgetfile.h
  httpclient.h
)

set(CPPS
  CFtp.cpp
  Chttpget.cpp
  inetgetfile.cpp
  httpclient.cpp
)

add_library(inetfile STATIC ${HEADERS} ${CPPS})
target_link_libraries(inetfile PRIVATE
  ddio
  mem
  httplib::httplib
)
target_include_directories(inetfile PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/netcon/inetfile
  >
)
