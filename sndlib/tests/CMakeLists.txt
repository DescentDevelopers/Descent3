d3_test_sources(sndlib_tests TO d3_tests
  SOURCES
    mixer_tests.cpp
    soundload_tests.cpp
    wave_load_tests.cpp
    support/mixer_test_support.cpp
    support/soundload_test_support.cpp
    support/wave_load_test_support.cpp
  LIBRARIES sndlib cfile ddebug ddio logger mem physics manage d3_test_game SDL3::SDL3
  INCLUDES ${PROJECT_SOURCE_DIR}/manage
)
