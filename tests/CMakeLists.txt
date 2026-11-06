include(D3Testing)
add_subdirectory(support)

# The executable that holds every test that can share one. A test that needs an executable of its own says why next to
# its d3_test_executable().
d3_test_executable(d3_tests)
target_link_libraries(d3_tests PRIVATE d3_test_game d3_test_renderer)

d3_test_sources(byteswap_tests TO d3_tests
  SOURCES byteswap_tests.cpp
  INCLUDES ${PROJECT_SOURCE_DIR}/lib
)
