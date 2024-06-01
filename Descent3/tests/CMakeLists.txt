add_executable(
  porting-tests
  porting-tests.cpp
)

target_link_libraries(
  porting-tests
  GTest::gtest_main
)

target_include_directories(porting-tests PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/model
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/manage
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/physics
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/bitmap
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/module
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/networking
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/ddio
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/win32
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/linux
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/2dlib
  >
)

gtest_discover_tests(porting-tests)
