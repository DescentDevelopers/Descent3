d3_test_sources(md5_tests TO d3_tests
  SOURCES md5_tests.cpp
  LIBRARIES md5
  INCLUDES ${PROJECT_SOURCE_DIR}/md5 ${PROJECT_SOURCE_DIR}/lib
)
