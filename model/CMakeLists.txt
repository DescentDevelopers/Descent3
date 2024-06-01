set(HEADERS
  polymodel_external.h
  polymodel.h)

set(CPPS
  newstyle.cpp
  polymodel.cpp)

add_library(model STATIC ${HEADERS} ${CPPS})
target_link_libraries(model PRIVATE
  ddio
)
target_include_directories(model PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/bitmap
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/renderer
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/networking
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/physics
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/manage
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/ddio
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/2dlib
  >
)
