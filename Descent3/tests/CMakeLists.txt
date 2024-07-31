set(CMAKE_FOLDER "tests")

add_executable(
  porting-tests
  porting-tests.cpp
)

target_link_libraries(
  porting-tests
  GTest::gtest_main
)

gtest_discover_tests(porting-tests)

add_executable(
  localization_tests
  localization_tests.cpp
  ../localization.cpp
)

target_link_libraries(
  localization_tests
  GTest::gtest_main
  cfile
  ddio
  mem
  misc
)

gtest_discover_tests(localization_tests
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Descent3/tests
)
