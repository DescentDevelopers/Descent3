set(CMAKE_FOLDER "tests")

add_executable(
  porting-tests
  porting-tests.cpp
)

target_link_libraries(
  porting-tests
  module
  GTest::gtest_main
)

gtest_discover_tests(porting-tests)

add_executable(
  gamespyutils_tests
  gamespyutils_tests.cpp
  ../gamespyutils.cpp
)
target_link_libraries(
  gamespyutils_tests
  GTest::gtest_main
)
gtest_discover_tests(gamespyutils_tests)

add_executable(
  LocDB_tests
  LocDB_tests.cpp
  ../LocDB.cpp
  ../MessageBank.cpp
)

target_link_libraries(
  LocDB_tests
  GTest::gtest_main
  cfile
  logger
  misc
)

gtest_discover_tests(LocDB_tests
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Descent3/tests
)