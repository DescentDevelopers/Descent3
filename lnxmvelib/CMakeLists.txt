set(HEADERS
  lnxdsound.h
  mvegfx.h
  mvelibi.h
  mvelibl.h
  snd8to16.h)

set(CPPS
  lnxdraw.cpp
  lnxdsound.cpp
  mveasm.cpp
  mvelibl.cpp)

#  asmstub.c

add_library(lnxmvelib STATIC ${HEADERS} ${CPPS})
