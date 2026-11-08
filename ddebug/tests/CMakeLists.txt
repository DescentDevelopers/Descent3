d3_test_sources(ddebug_tests TO d3_tests
  SOURCES ddebug_tests.cpp support/fake_message_boxes.cpp support/test_predicates.cpp
  LIBRARIES ddebug SDL3::SDL3 logger
)
