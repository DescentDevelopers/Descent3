# The renderer needs an OpenGL context, so the 3D pipeline these tests cover runs on top of the fake backend in
# d3_test_renderer (tests/support/renderer).
d3_test_sources(renderer_tests TO d3_tests
  SOURCES projection_tests.cpp clipper_tests.cpp instance_tests.cpp support/renderer_checks.cpp
    support/renderer_helpers.cpp
  LIBRARIES d3_test_renderer vecmat ddebug logger misc
  INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/..
)
