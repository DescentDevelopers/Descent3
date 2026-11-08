d3_test_sources(manage_tests TO d3_tests
  SOURCES
    page_tests.cpp
    pagelock_tests.cpp
    support/manage_page_builders.cpp
    support/manage_page_io.cpp
    support/manage_pagelock_records.cpp
  LIBRARIES manage cfile ddebug ddio logger mem model sndlib stream_audio d3_test_game d3_test_renderer
  INCLUDES ${PROJECT_SOURCE_DIR}/manage
)
