set(HEADERS
  grtext.h
  grtextlib.h
)
set(CPPS
  grfont.cpp
  grtext.cpp
  textaux.cpp
)

add_library(grtext STATIC ${HEADERS} ${CPPS})
target_link_libraries(grtext PRIVATE
  ddebug
  ddio
  logger
  mem
)
target_include_directories(grtext PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/grtext
  >
)
