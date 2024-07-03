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
