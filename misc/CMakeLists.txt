set(HEADERS)
set(CPPS
  error.cpp
  logfile.cpp
  log.cpp
  psglob.cpp
  psrand.cpp
  pstring.cpp)

add_library(misc STATIC ${HEADERS} ${CPPS})

target_link_libraries(misc ddebug)

target_include_directories(misc PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/ddio
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/win32
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/linux
  >
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
