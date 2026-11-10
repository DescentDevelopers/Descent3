# networking.cpp references a handful of symbols that live in the Descent3 executable: the command line and the app
# database come from d3_test_game, and on Windows the directplay.cpp functions (which in turn need the game's player
# tables) are stubbed in networking_test_stubs.cpp.
d3_test_sources(networking_tests TO d3_tests
  SOURCES
    networking_tests.cpp
    support/networking_test_packets.cpp
    support/networking_test_reliable.cpp
    support/networking_test_stubs.cpp
    support/networking_test_unreliable.cpp
  LIBRARIES networking cfile d3_test_game ddio SDL3::SDL3 $<$<PLATFORM_ID:Windows>:wsock32>
)
