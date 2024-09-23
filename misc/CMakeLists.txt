set(HEADERS
  psglob.h
  psrand.h
  pstring.h
)
set(CPPS
  psglob.cpp
  psrand.cpp
  pstring.cpp
)

add_library(misc STATIC ${HEADERS} ${CPPS})
target_include_directories(misc PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/misc
  >
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
