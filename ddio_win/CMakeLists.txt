set(HEADERS ddio_win.h)
set(CPPS
  serial.cpp
  winfile.cpp
  winforcefeedback.cpp
  winio.cpp
  winjoy.cpp
  winkey.cpp
  winmouse.cpp
  wintimer.cpp)

add_library(ddio_win STATIC ${HEADERS} ${CPPS})
