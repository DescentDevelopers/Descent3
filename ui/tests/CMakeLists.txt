d3_test_sources(ui_tests TO d3_tests
  SOURCES
    uiedit_tests.cpp
    uilistbox_tests.cpp
    uiwindow_tests.cpp
    uicombo_slider_tests.cpp
    support/ui_test_util.cpp
  LIBRARIES ui grtext ddebug ddio logger mem d3_test_game d3_test_renderer
  INCLUDES ${PROJECT_SOURCE_DIR}/ui ${PROJECT_SOURCE_DIR}/grtext
)
