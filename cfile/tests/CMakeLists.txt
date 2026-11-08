d3_test_sources(cfile_tests TO d3_tests
  SOURCES cfile_tests.cpp cfile_io_tests.cpp cfile_hog_tests.cpp inffile_tests.cpp
  LIBRARIES cfile
  INCLUDES ${PROJECT_SOURCE_DIR}/lib
  # The directory that holds TestDir/, the files the tests read.
  DEFINITIONS CFILE_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
