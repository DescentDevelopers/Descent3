d3_test_sources(physics_tests TO d3_tests
  SOURCES
    fvi_geometry_tests.cpp
    physics_sim_tests.cpp
    support/fvi_checks.cpp
    support/sim_object.cpp
    support/vector_helpers.cpp
  LIBRARIES physics model rtperformance ddebug fix logger mem misc vecmat d3_test_game d3_test_renderer
  INCLUDES ${PROJECT_SOURCE_DIR}/physics ${PROJECT_SOURCE_DIR}/model ${PROJECT_SOURCE_DIR}/rtperformance
)
