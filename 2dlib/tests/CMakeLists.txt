d3_test_sources(2dlib_tests TO d3_tests
  SOURCES surface_tests.cpp text_tests.cpp support/surface.cpp support/test_font.cpp support/text_draw.cpp
  LIBRARIES 2dlib bitmap cfile ddebug d3_test_game d3_test_renderer
)
