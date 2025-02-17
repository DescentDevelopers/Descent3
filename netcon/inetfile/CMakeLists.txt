set(HEADERS
  httpclient.h
)

set(CPPS
  httpclient.cpp
)

add_library(inetfile STATIC ${HEADERS} ${CPPS})
target_link_libraries(inetfile PRIVATE
  ddio
)
target_link_libraries(inetfile PUBLIC
  httplib::httplib # For some reason, linking it privately causes issues at runtime
)
target_include_directories(inetfile PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/netcon/inetfile
  >
)
