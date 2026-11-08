# A tiny loadable module used as a fixture by module_tests. It exports
# symbols the same way the game's DLLs do (DLLEXPORT / DLLFUNCCALL), and is
# named the way the game's own modules are (see scripts/CMakeLists.txt) so its
# file extension is the platform's MODULE_EXT.
add_library(module_test_fixture MODULE
  support/test_fixture_module.cpp
)
set_target_properties(module_test_fixture PROPERTIES FOLDER "tests")
target_include_directories(module_test_fixture PRIVATE ${PROJECT_SOURCE_DIR}/module)
set_target_properties(module_test_fixture PROPERTIES PREFIX "" CXX_VISIBILITY_PRESET "hidden")
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set_target_properties(module_test_fixture PROPERTIES SUFFIX ".dylib")
endif()

d3_test_sources(module_tests TO d3_tests
  SOURCES module_tests.cpp
  LIBRARIES module
  DEFINITIONS "MODULE_TEST_FIXTURE_PATH=\"$<TARGET_FILE:module_test_fixture>\""
)
add_dependencies(d3_tests module_test_fixture)
