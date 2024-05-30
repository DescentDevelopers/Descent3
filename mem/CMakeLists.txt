set(HEADERS
  ../lib/mem.h)

set(CPPS
  mem.cpp)

add_library(mem STATIC ${HEADERS} ${CPPS})
