SET (HEADERS dyna_pthread.h  dyna_xext.h  dyna_xwin.h  lnxdsound.h  mvegfx.h  mvelibi.h  mvelibl.h  snd8to16.h  )
SET (CPPS
  lnxdraw.cpp
  lnxdsound.cpp
  mveasm.cpp
  mvelibl.cpp)

#  asmstub.c

ADD_LIBRARY(lnxmvelib STATIC ${HEADERS} ${CPPS})