set(CMAKE_FOLDER "tests")

add_executable(ddio_tests
  ddio_tests.cpp
)
target_link_libraries(ddio_tests PRIVATE
  GTest::gtest_main
  ddio
)
gtest_discover_tests(ddio_tests)
