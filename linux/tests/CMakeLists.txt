d3_test_sources(linux_tests TO d3_tests
  SOURCES registry_tests.cpp lnxdata_tests.cpp support/registry_records.cpp
  LIBRARIES linux ddebug logger SDL3::SDL3
  INCLUDES ${PROJECT_SOURCE_DIR}/linux ${PROJECT_BINARY_DIR}/lib
)
