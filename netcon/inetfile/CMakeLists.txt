set(HEADERS
  ../includes/CFtp.h
  ../includes/Chttpget.h
  ../includes/inetgetfile.h
)

set(CPPS
  CFtp.cpp
  Chttpget.cpp
  inetgetfile.cpp)

add_library(inetfile STATIC ${HEADERS} ${CPPS})
