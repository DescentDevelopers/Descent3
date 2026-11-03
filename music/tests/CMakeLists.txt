d3_test_sources(music_tests TO d3_tests
  SOURCES music_tests.cpp support/music_seq_harness.cpp
  LIBRARIES music SDL3::SDL3
)
