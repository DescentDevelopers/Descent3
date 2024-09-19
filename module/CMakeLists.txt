set(HEADERS
  module.h
)
set(CPPS
  module.cpp
)

add_library(module STATIC ${HEADERS} ${CPPS})
target_link_libraries(module PRIVATE
  cfile
  ddebug
  logger
)
target_include_directories(module PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/module
  >
)