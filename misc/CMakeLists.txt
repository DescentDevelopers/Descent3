set(HEADERS)
set(CPPS
  error.cpp
  logfile.cpp
  log.cpp
  psglob.cpp
  psrand.cpp
  pstring.cpp)

add_library(misc STATIC ${HEADERS} ${CPPS})

target_link_libraries(misc spdlog_header ddebug)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
