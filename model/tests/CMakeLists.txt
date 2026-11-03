d3_test_sources(model_tests TO d3_tests
  SOURCES polymodel_tests.cpp support/polymodel_harness.cpp support/test_stubs.cpp
  LIBRARIES model manage cfile ddebug fix logger mem misc vecmat d3_test_game d3_test_renderer
  INCLUDES ${PROJECT_SOURCE_DIR}/model
)
