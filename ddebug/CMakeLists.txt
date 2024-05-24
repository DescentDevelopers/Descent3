set(CPPS
  $<$<PLATFORM_ID:Darwin,Linux>:
    lnxdebug.cpp
    lnxmono.cpp
  >
  $<$<PLATFORM_ID:Windows>:
    windebug.cpp
    winmono.cpp
  >
)

add_library(ddebug STATIC ${CPPS})
target_include_directories(ddebug PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/ddebug
  >
  PRIVATE ${PROJECT_BINARY_DIR}/lib   # For d3_version.h
)
