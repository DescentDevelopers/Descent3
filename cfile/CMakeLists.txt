set(HEADERS)
set(CPPS
  CFILE.cpp
  hog.cpp
  InfFile.cpp)

add_library(cfile STATIC ${HEADERS} ${CPPS})
