# The main() every test executable runs with.
add_library(d3_test_main STATIC test_main.cpp)
set_target_properties(d3_test_main PROPERTIES FOLDER "tests")
target_link_libraries(d3_test_main PUBLIC GTest::gtest ddebug SDL3::SDL3)

# Helpers shared by the tests of several modules.
add_library(d3_test_support STATIC
  d3_test/byte_writer.h
  d3_test/files.h
  d3_test/game_assertions.h
  d3_test/game_clock.h
  d3_test/sockets.h
  files.cpp
  game_assertions.cpp
)
set_target_properties(d3_test_support PROPERTIES FOLDER "tests")
target_include_directories(d3_test_support PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(d3_test_support PUBLIC GTest::gtest SDL3::SDL3)

# The two libraries below are object libraries, linked straight into the test executables that need them
# (tests/CMakeLists.txt): the game libraries reference their symbols, and an archive would have to come after every one
# of those libraries on the link line.

# The parts of the Descent3 executable that the game libraries need: the real command-line handling and string
# tables (args.cpp, localization.cpp), and stand-ins for the executable's other globals and functions.
add_library(d3_test_game OBJECT
  ${PROJECT_SOURCE_DIR}/Descent3/args.cpp
  ${PROJECT_SOURCE_DIR}/Descent3/localization.cpp
  d3_test/game.h
  game/game_functions.cpp
  game/game_globals.cpp
)
set_target_properties(d3_test_game PROPERTIES FOLDER "tests")
target_link_libraries(d3_test_game PUBLIC d3_test_support bitmap cfile ddebug ddio fix logger mem misc model module physics
  sndlib vecmat)

# The renderer the tests run against: the real device-independent 3D pipeline on top of a fake of the OpenGL backend
# and of the platform video layer.
add_library(d3_test_renderer OBJECT
  ${PROJECT_SOURCE_DIR}/renderer/HardwareClipper.cpp
  ${PROJECT_SOURCE_DIR}/renderer/HardwareDraw.cpp
  ${PROJECT_SOURCE_DIR}/renderer/HardwareGlobalVars.cpp
  ${PROJECT_SOURCE_DIR}/renderer/HardwareInstance.cpp
  ${PROJECT_SOURCE_DIR}/renderer/HardwarePoints.cpp
  ${PROJECT_SOURCE_DIR}/renderer/HardwareSetup.cpp
  ${PROJECT_SOURCE_DIR}/renderer/HardwareTransforms.cpp
  d3_test/renderer.h
  renderer/fake_renderer.cpp
)
set_target_properties(d3_test_renderer PROPERTIES FOLDER "tests")
target_include_directories(d3_test_renderer PRIVATE ${PROJECT_SOURCE_DIR}/renderer)
# The platform video layer (ddgr) is dd_grwin32 where that is built (Windows, with BUILD_EDITOR); elsewhere the fake
# stands in for it too.
target_compile_definitions(d3_test_renderer PRIVATE $<$<NOT:$<TARGET_EXISTS:dd_grwin32>>:D3_TEST_FAKE_DDGR>)
target_link_libraries(d3_test_renderer PUBLIC $<$<TARGET_EXISTS:dd_grwin32>:dd_grwin32>)
target_link_libraries(d3_test_renderer PUBLIC d3_test_support bitmap ddebug logger misc vecmat)
