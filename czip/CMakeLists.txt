set(HEADERS
  ../lib/CZip.h
)
set(CPPS
  BitIO.cpp
  CZip.cpp
  HuffmanAdapt.cpp
  HuffmanBasic.cpp)

add_library(czip STATIC ${HEADERS} ${CPPS})
