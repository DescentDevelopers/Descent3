SET (HEADERS )

SET (CPPS 
  CFtp.cpp
  Chttpget.cpp
  inetgetfile.cpp
)

ADD_LIBRARY(inetfile STATIC ${HEADERS} ${CPPS})
