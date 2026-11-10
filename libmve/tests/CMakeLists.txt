d3_test_sources(libmve_tests TO d3_tests
  SOURCES decoder8_tests.cpp decoder16_tests.cpp mve_audio_tests.cpp mvelib_tests.cpp mveplay_tests.cpp
  LIBRARIES ddio libmve SDL3::SDL3
)
