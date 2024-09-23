set(HEADERS
  debug.h
  mono.h
  pserror.h
)
set(CPPS
  debug.cpp
  error.cpp
  $<$<PLATFORM_ID:Darwin,Linux,OpenBSD>:
    lnxdebug.cpp
    lnxmono.cpp
  >
  $<$<PLATFORM_ID:Windows>:
    windebug.cpp
    winmono.cpp
  >
)

add_library(ddebug STATIC ${HEADERS} ${CPPS})
add_dependencies(ddebug get_git_hash)
target_include_directories(ddebug PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/ddebug
  >
  PRIVATE ${PROJECT_BINARY_DIR}/lib   # For d3_version.h
)
target_link_libraries(ddebug PRIVATE
  $<$<PLATFORM_ID:Windows>:
    mem
    misc
  >
  PUBLIC
  SDL2::SDL2
  logger
)
