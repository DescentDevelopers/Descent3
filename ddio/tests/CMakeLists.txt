d3_test_sources(ddio_tests TO d3_tests
  SOURCES ddio_tests.cpp
  LIBRARIES ddio
)

# ddio contains one of the game's two clocks, the one ENABLE_MOCK_CLOCK picks, and its tests run in d3_tests. The
# other clock is compiled into clock_tests together with its tests, so both clocks are tested in every build.
d3_test_executable(clock_tests)
if(ENABLE_MOCK_CLOCK)
  d3_test_sources(mock_timer_tests TO d3_tests
    SOURCES mock_timer_tests.cpp
    LIBRARIES ddio
    INCLUDES ${PROJECT_SOURCE_DIR}/ddio
  )
  d3_test_sources(chrono_timer_tests TO clock_tests
    SOURCES chrono_timer_tests.cpp ../chrono_timer.cpp
    INCLUDES ${PROJECT_SOURCE_DIR}/ddio
  )
else()
  d3_test_sources(chrono_timer_tests TO d3_tests
    SOURCES chrono_timer_tests.cpp
    LIBRARIES ddio
    INCLUDES ${PROJECT_SOURCE_DIR}/ddio
  )
  d3_test_sources(mock_timer_tests TO clock_tests
    SOURCES mock_timer_tests.cpp ../mock_timer.cpp
    INCLUDES ${PROJECT_SOURCE_DIR}/ddio
    DEFINITIONS MOCK_CLOCK
  )
endif()
