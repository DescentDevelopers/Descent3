d3_test_sources(grtext_tests TO d3_tests
  SOURCES
    grfont_tests.cpp
    grtext_tests.cpp
    textaux_tests.cpp
    support/grtext_test_support.cpp
    support/test_fonts.cpp
    support/textaux_test_support.cpp
  LIBRARIES grtext bitmap cfile ddebug ddio logger mem d3_test_game d3_test_renderer
)
