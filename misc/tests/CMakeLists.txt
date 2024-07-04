set(CMAKE_FOLDER "tests")

add_executable(
  misc_tests
  misc_tests.cpp
)
target_link_libraries(
  misc_tests
  GTest::gtest_main
  misc
)
gtest_discover_tests(misc_tests)
