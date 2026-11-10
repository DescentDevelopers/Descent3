# Each test creates (and removes) its own subdirectory here. A fixed location is
# needed because death-test children may re-run the test body in a new process.
d3_test_sources(logger_tests TO d3_tests
  SOURCES logger_tests.cpp support/logger_test_files.cpp
  LIBRARIES logger
  DEFINITIONS LOGGER_TESTS_SCRATCH_DIR="${CMAKE_CURRENT_BINARY_DIR}/scratch"
)
