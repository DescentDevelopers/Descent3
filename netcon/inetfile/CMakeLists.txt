set(HEADERS)

set(CPPS 
  CFtp.cpp
  Chttpget.cpp
  inetgetfile.cpp
)

add_library(inetfile STATIC ${HEADERS} ${CPPS})
