set(HEADERS)
set(CPPS
  error.cpp
  logfile.cpp
  psglob.cpp
  psrand.cpp
  pstring.cpp)

add_library(misc STATIC ${HEADERS} ${CPPS})
target_link_libraries(misc
  ddebug
)
