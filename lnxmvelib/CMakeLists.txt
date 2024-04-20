set(HEADERS dyna_pthread.h  dyna_xext.h  dyna_xwin.h  lnxdsound.h  mvegfx.h  mvelibi.h  mvelibl.h  snd8to16.h  )
set(CPPS
  lnxdraw.cpp
  lnxdsound.cpp
  mveasm.cpp
  mvelibl.cpp)

#  asmstub.c

add_library(lnxmvelib STATIC ${HEADERS} ${CPPS})