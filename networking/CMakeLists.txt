set(HEADERS
  networking.h)
set(CPPS
  networking.cpp)

if(WIN32)
  set(HEADERS ${HEADERS} directplay.h)
  set(CPPS ${CPPS} directplay.cpp)
endif()

add_library(networking STATIC ${HEADERS} ${CPPS})
target_link_libraries(networking PRIVATE
  ddio
)

target_include_directories(networking PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/renderer
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/bitmap
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/manage
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/module
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/model
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/ddio
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/2dlib
  >
)
