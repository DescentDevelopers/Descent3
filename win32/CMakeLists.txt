set(CPPS
  winapp.cpp
  wincon.cpp
  WinController.cpp
  windata.cpp
  wintask.cpp
)

add_library(win32 STATIC ${CPPS})
