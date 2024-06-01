set(HEADERS
  grtext.h
  grtextlib.h)

set(CPPS
  grfont.cpp
  grtext.cpp
  textaux.cpp)

add_library(grtext STATIC ${HEADERS} ${CPPS})
target_link_libraries(grtext PRIVATE
  ddio
)

target_include_directories(grtext PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/bitmap
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/renderer
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/ddio
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/dd_video
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/2dlib
  >
)
