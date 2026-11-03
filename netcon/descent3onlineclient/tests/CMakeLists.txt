d3_test_sources(Descent3_Online_TCP_IP_tests TO netcon_tests
  SOURCES dip_gametrack_tests.cpp support/dip_test_helpers.cpp ../dip_gametrack.cpp
  LIBRARIES GTest::gmock inetfile logger $<$<PLATFORM_ID:Windows>:ws2_32>
  INCLUDES ${PROJECT_SOURCE_DIR}/netcon/descent3onlineclient
)
