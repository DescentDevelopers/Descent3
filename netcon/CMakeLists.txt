set(CMAKE_FOLDER "netcon")

include_directories("includes")

if(BUILD_TESTING)
  # The tests of the online modules. Each module defines the game-to-module API (con_dll.h) itself, so they cannot share
  # an executable with the rest of the tests; here it is defined once, by lanclient.cpp, and the other modules' tests
  # point it at their fakes.
  d3_test_executable(netcon_tests)
endif()
add_subdirectory(chat)
add_subdirectory(inetfile)
add_subdirectory(lanclient)
add_subdirectory(mtclient)
add_subdirectory(descent3onlineclient)
