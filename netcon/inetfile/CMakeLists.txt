set(HEADERS
  httpclient.h
)

set(CPPS
  httpclient.cpp
)

add_library(inetfile STATIC ${HEADERS} ${CPPS})
target_link_libraries(inetfile PRIVATE
  ddio
  httplib::httplib
)
target_include_directories(inetfile PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/netcon/inetfile
  >
)
