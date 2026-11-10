set(HEADERS
  chrono_timer.h
  chrono_timer_internal.h
  mock_timer_internal.h
  ddio.h
  ddio_common.h
  ddio_lnx.h
  lnxkey_sdl.h
  controller.h
  sdlcontroller.h
)
set(CPPS
  $<IF:$<BOOL:${ENABLE_MOCK_CLOCK}>,mock_timer.cpp,chrono_timer.cpp>
  ddio.cpp
  file.cpp
  key.cpp
  lnxforcefeedback.cpp
  lnxio.cpp
  sdljoy.cpp
  lnxkey.cpp
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
  SDL3::SDL3
  ddebug
  logger
)
target_include_directories(ddio PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/ddio
  >
  ${SDL3_INCLUDE_DIRS}
)
target_include_directories(ddio PRIVATE ${PROJECT_BINARY_DIR}/lib)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
