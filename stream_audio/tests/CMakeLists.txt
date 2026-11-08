d3_test_sources(stream_audio_tests TO d3_tests
  SOURCES osfarchive_tests.cpp audiostream_tests.cpp support/osf_writer.cpp support/test_stream.cpp
  # sndlib provides ssl_lib.h and the llsSystem base class used by the fake sound system
  LIBRARIES stream_audio sndlib cfile ddio mem
)
