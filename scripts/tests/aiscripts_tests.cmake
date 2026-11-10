# Script modules are MODULE libraries and cannot be linked against, so each test
# executable compiles one script source in directly (through a small
# aiscripts_<name>_module.cpp adapter that #includes it). Every script defines the
# same exported entry points (InitializeDLL, GetGOScriptID, ...), so each script
# needs its own test executable.

function(aiscripts_add_test name)
  d3_test_executable(${name} TEST_PREFIX "${name}.")
  d3_test_sources(${name}_sources TO ${name}
    SOURCES
      ${ARGN}
      support/aiscripts_test_support.cpp
      $<$<PLATFORM_ID:Darwin,Linux,OpenBSD>:${PROJECT_SOURCE_DIR}/scripts/linux_lib.cpp>
    LIBRARIES cfile dallas fix misc module
    INCLUDES ${PROJECT_SOURCE_DIR}/scripts
  )
endfunction()

aiscripts_add_test(aiscripts_aigame_tests support/aiscripts_aigame_module.cpp aiscripts_factory_tests.cpp)
aiscripts_add_test(aiscripts_aigame2_tests support/aiscripts_aigame2_module.cpp aiscripts_factory_tests.cpp)
aiscripts_add_test(aiscripts_aigame3_tests support/aiscripts_aigame3_module.cpp aiscripts_factory_tests.cpp)
aiscripts_add_test(aiscripts_aigame4_tests support/aiscripts_aigame4_module.cpp aiscripts_factory_tests.cpp)

aiscripts_add_test(aiscripts_barney_tests support/aiscripts_barney_module.cpp support/aiscripts_message_files.cpp
                   aiscripts_message_file_tests.cpp)
aiscripts_add_test(aiscripts_y2k_tests support/aiscripts_y2k_module.cpp support/aiscripts_message_files.cpp
                   aiscripts_message_file_tests.cpp)
