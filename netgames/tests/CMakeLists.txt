set(CMAKE_FOLDER "tests")

# Netgame modes are MODULE targets and cannot be linked against, so each mode gets its own test
# executable: the test file #includes the mode's main source (making its file-local helpers
# reachable), any other sources of the mode are compiled in, and the static dmfc framework the
# mode builds on is linked as usual.
function(add_netgame_test mode)
  set(target netgame_${mode}_tests)
  d3_test_executable(${target} TEST_PREFIX "${mode}." NO_PRETTY_VALUES)
  d3_test_sources(${target}_sources TO ${target}
    SOURCES ${mode}_tests.cpp support/ranking_contract.cpp ${ARGN}
    LIBRARIES dmfc
    INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/../${mode}
  )
endfunction()

add_netgame_test(anarchy)
add_netgame_test(coop)
add_netgame_test(ctf)
add_netgame_test(entropy ../entropy/EntropyPackets.cpp ../entropy/EntropyRoom.cpp support/vector_helpers.cpp)
add_netgame_test(hoard ../hoard/hoard_ui.cpp)
add_netgame_test(hyperanarchy)
add_netgame_test(monsterball)
add_netgame_test(roboanarchy)
add_netgame_test(tanarchy)
