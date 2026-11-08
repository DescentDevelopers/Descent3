# Direct_TCP_IP is a MODULE library and cannot be linked. lanclient_tests.cpp
# #includes lanclient.cpp so the tests can reach the game-API pointers defined
# in con_dll.h and replace them with fakes.
d3_test_sources(lanclient_tests TO netcon_tests
  SOURCES lanclient_tests.cpp
  LIBRARIES ddio inetfile logger misc module ui SDL3::SDL3 $<$<PLATFORM_ID:Windows>:ws2_32>
  INCLUDES ${PROJECT_SOURCE_DIR}/netcon/lanclient
)
