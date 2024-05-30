set(HEADERS
  ../lib/log.h
  ../lib/logfile.h
  ../lib/psglob.h
  ../lib/psrand.h
  ../lib/pstring.h
)

set(CPPS
  error.cpp
  logfile.cpp
  log.cpp
  psglob.cpp
  psrand.cpp
  pstring.cpp)

add_library(misc STATIC ${HEADERS} ${CPPS})

target_link_libraries(misc ddebug)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
