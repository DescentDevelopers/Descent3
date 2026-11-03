# The editor is built with EDITOR defined, which changes what the game headers declare, so its tests need an
# executable of their own. The editor executable cannot be linked into a test, so the editor/game globals and
# functions these sources reference are provided by editor_test_stubs.cpp.
d3_test_executable(editor_tests)
d3_test_sources(editor_test_sources TO editor_tests
  SOURCES
    room_geometry_tests.cpp
    support/editor_test_stubs.cpp
    support/room_test_expect.cpp
    support/room_test_rooms.cpp
    ../Erooms.cpp
    ../RoomUVs.cpp
    ../../Descent3/room.cpp
  LIBRARIES cfile ddebug logger mem misc vecmat
  INCLUDES
    ${PROJECT_SOURCE_DIR}/editor
    ${PROJECT_SOURCE_DIR}/Descent3
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/manage
    ${PROJECT_SOURCE_DIR}/physics
    ${PROJECT_SOURCE_DIR}/2dlib
    ${PROJECT_BINARY_DIR}/lib
  DEFINITIONS EDITOR
)
