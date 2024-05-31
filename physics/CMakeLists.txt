set(HEADERS
  collide.h
  findintersection.h
  physics.h)

set(CPPS
  collide.cpp
  findintersection.cpp
  newstyle_fi.cpp
  physics.cpp)

add_library(physics STATIC ${HEADERS} ${CPPS})
target_link_libraries(physics PRIVATE
  ddio
  sndlib
)

target_include_directories(physics PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/rtperformance
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/module
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/model
  >
)
