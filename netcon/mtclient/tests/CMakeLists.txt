d3_test_sources(Parallax_Online_tests TO netcon_tests
  SOURCES
    gametrack_tests.cpp
    pilottrack_tests.cpp
    support/gametrack_packets.cpp
    support/pilottrack_packets.cpp
    support/stubs.cpp
    ../mt_net.cpp
    ../mtgametrack.cpp
    ../mtpilottracker.cpp
  LIBRARIES logger SDL3::SDL3 $<$<PLATFORM_ID:Windows>:ws2_32>
  INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..
)
