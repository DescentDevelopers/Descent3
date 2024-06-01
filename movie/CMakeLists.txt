set(HEADERS
  movie.h)
set(CPPS
  movie.cpp)

add_library(movie STATIC ${HEADERS} ${CPPS})
target_link_libraries(movie PRIVATE
  ddio
  libmve
)

target_include_directories(movie PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/renderer
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/grtext
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/bitmap
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/model
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/manage
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/ddio
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/dd_video
  >
)
