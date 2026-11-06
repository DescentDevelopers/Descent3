d3_test_sources(porting_tests TO d3_tests
  SOURCES porting-tests.cpp
  LIBRARIES module
)

d3_test_sources(gamespyutils_tests TO d3_tests
  SOURCES gamespyutils_tests.cpp ../gamespyutils.cpp
)
