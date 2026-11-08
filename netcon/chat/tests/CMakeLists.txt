d3_test_sources(netcon_chat_tests TO netcon_tests
  SOURCES chat_api_tests.cpp support/chat_module_fakes.cpp support/chat_test_helpers.cpp
  LIBRARIES netcon_chat GTest::gmock logger
)
