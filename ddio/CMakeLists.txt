set(HEADERS
  chrono_timer.h
  ddio.h
  ddio_common.h
  ddio_lnx.h
  ddio_win.h)
set(CPPS
  chrono_timer.cpp
  ddio.cpp
  file.cpp
  key.cpp
  lnxforcefeedback.cpp
  lnxio.cpp
  sdljoy.cpp
  lnxkey.cpp
  lnxkey_null.cpp
  lnxmouse.cpp
  lnxkey_sdl.cpp
  $<$<PLATFORM_ID:Darwin,Linux,OpenBSD>:
    lnxfile.cpp
  >
  $<$<PLATFORM_ID:Windows>:
    winfile.cpp
  >
)
add_library(ddio STATIC ${HEADERS} ${CPPS})
target_link_libraries(ddio PRIVATE
  SDL2::SDL2
  ddebug
  mem
  misc
)
target_include_directories(ddio PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/ddio
  >
)
