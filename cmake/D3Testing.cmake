# Test executables, and the test code compiled into them. See "Running the tests" in BUILD.md.
#
# Every test executable runs with the shared main() in tests/support/test_main.cpp, which turns the game's exit(0),
# assertion and dialog paths into test failures.

# d3_test_executable(<name> [WORKING_DIRECTORY <dir>] [TEST_PREFIX <prefix>] [NO_PRETTY_VALUES])
#   Creates the test executable <name> and registers every test case in it with CTest. TEST_PREFIX and
#   NO_PRETTY_VALUES are passed on to gtest_discover_tests().
function(d3_test_executable name)
  cmake_parse_arguments(PARSE_ARGV 1 arg "NO_PRETTY_VALUES" "WORKING_DIRECTORY;TEST_PREFIX" "")
  add_executable(${name})
  set_target_properties(${name} PROPERTIES FOLDER "tests")
  target_link_libraries(${name} PRIVATE d3_test_main)
  set(discover_args DISCOVERY_TIMEOUT 60)
  if(arg_WORKING_DIRECTORY)
    list(APPEND discover_args WORKING_DIRECTORY ${arg_WORKING_DIRECTORY})
  endif()
  if(arg_TEST_PREFIX)
    list(APPEND discover_args TEST_PREFIX ${arg_TEST_PREFIX})
  endif()
  if(arg_NO_PRETTY_VALUES)
    list(APPEND discover_args NO_PRETTY_VALUES)
  endif()
  gtest_discover_tests(${name} ${discover_args})
endfunction()

# d3_test_sources(<name> TO <executable> SOURCES <file>... [LIBRARIES <lib>...] [INCLUDES <dir>...]
#                 [DEFINITIONS <definition>...])
#   Compiles one module's tests as the object library <name>, with that module's include directories and
#   definitions, and links them into <executable>. Most modules add theirs to d3_tests, the executable that holds
#   every test that can share one.
function(d3_test_sources name)
  cmake_parse_arguments(PARSE_ARGV 1 arg "" "TO" "SOURCES;LIBRARIES;INCLUDES;DEFINITIONS")
  add_library(${name} OBJECT ${arg_SOURCES})
  set_target_properties(${name} PROPERTIES FOLDER "tests")
  target_link_libraries(${name} PUBLIC d3_test_support ${arg_LIBRARIES})
  if(arg_INCLUDES)
    target_include_directories(${name} PRIVATE ${arg_INCLUDES})
  endif()
  if(arg_DEFINITIONS)
    target_compile_definitions(${name} PRIVATE ${arg_DEFINITIONS})
  endif()
  target_link_libraries(${arg_TO} PRIVATE ${name})
endfunction()
