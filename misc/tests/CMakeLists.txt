d3_test_sources(misc_tests TO d3_tests
  SOURCES holder_tests.cpp misc_tests.cpp psglob_tests.cpp psrand_tests.cpp support/psglob_match.cpp
    support/psrand_draw.cpp
  LIBRARIES misc
)
