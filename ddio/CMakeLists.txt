set(HEADERS
  chrono_timer.h
  ddio.h
  ddio_common.h
  ddio_lnx.h
  controller.h
  sdlcontroller.h
)
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
  sdlcontroller.cpp
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
  logger
  mem
)
target_include_directories(ddio PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/ddio
  >
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
