# Run-time profiling is only compiled in on Windows (see USE_RTP in rtperformance.h); on other platforms the whole
# library is a set of no-ops, so there is nothing to test there.
if(WIN32)
  d3_test_sources(rtperformance_tests TO d3_tests
    SOURCES rtperformance_tests.cpp support/rtp_log.cpp
    LIBRARIES rtperformance cfile ddio manage
  )
endif()
