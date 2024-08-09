set(HEADERS md5.h)
set(CPPS
  md5.cpp)

add_library(md5 STATIC ${HEADERS} ${CPPS})

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
