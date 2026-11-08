d3_test_sources(porting_tests TO d3_tests
  SOURCES porting-tests.cpp
  LIBRARIES module
)

d3_test_sources(gamespyutils_tests TO d3_tests
  SOURCES gamespyutils_tests.cpp ../gamespyutils.cpp
)

# args.cpp and localization.cpp themselves are compiled into d3_test_game, which provides the game executable's command
# line and string tables to every test.
d3_test_sources(args_tests TO d3_tests
  SOURCES args_tests.cpp support/gather_args.cpp
  LIBRARIES d3_test_game logger
)

d3_test_sources(list_tests TO d3_tests
  SOURCES list_tests.cpp support/list_walk.cpp ../list.cpp
  LIBRARIES SDL3::SDL3 ddebug logger mem
)

d3_test_sources(localization_tests TO d3_tests
  SOURCES localization_tests.cpp support/string_table.cpp
  LIBRARIES cfile d3_test_game ddebug ddio logger mem
)

d3_test_sources(briefingparse_tests TO d3_tests
  SOURCES briefingparse_tests.cpp support/briefing_parse.cpp support/briefing_recorder.cpp ../BriefingParse.cpp
  LIBRARIES cfile ddebug grtext logger mem
)

# Header-only helpers; the libraries are linked only for their include directories.
d3_test_sources(multi_pack_tests TO d3_tests
  SOURCES multi_pack_tests.cpp
  LIBRARIES networking
)

d3_test_sources(slider_math_tests TO d3_tests
  SOURCES slider_math_tests.cpp support/slider_settings.cpp
  LIBRARIES grtext networking ui
)
