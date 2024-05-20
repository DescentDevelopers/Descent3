add_executable(
  md5_tests
  ../md5.cpp
  md5_tests.cpp
)
target_link_libraries(
  md5_tests
  GTest::gtest_main
)
target_include_directories(md5_tests PRIVATE ${PROJECT_SOURCE_DIR}/md5 ${PROJECT_SOURCE_DIR}/lib)

gtest_discover_tests(md5_tests)
