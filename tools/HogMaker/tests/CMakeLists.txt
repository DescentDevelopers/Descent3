# The game's own HOG reader (cfile) is used as the independent decoder.
d3_test_sources(hogmaker_tests TO d3_tests
  SOURCES hogmaker_tests.cpp support/hogmaker_test_support.cpp ${PROJECT_SOURCE_DIR}/tools/HogMaker/HogFormat.cpp
  LIBRARIES cfile
  INCLUDES ${PROJECT_SOURCE_DIR}/tools/HogMaker ${PROJECT_SOURCE_DIR}/lib
  DEFINITIONS HOGMAKER_EXE="$<TARGET_FILE:HogMaker>"
)
add_dependencies(d3_tests HogMaker)
