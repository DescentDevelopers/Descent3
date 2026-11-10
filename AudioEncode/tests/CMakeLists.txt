d3_test_sources(AudioEncode_tests TO d3_tests
  SOURCES
    audio_encode_tests.cpp
    support/acm_io.cpp
    support/signal_gen.cpp
  LIBRARIES AudioEncode libacm logger
)
