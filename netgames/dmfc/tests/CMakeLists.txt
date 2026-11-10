# dmfc has a CRegistry class of its own (dmfccfg.cpp) that differs from the linux library's, which the other tests
# link, so its tests need an executable of their own.
d3_test_executable(dmfc_tests)
d3_test_sources(dmfc_test_sources TO dmfc_tests
  SOURCES
    dmfcbase_tests.cpp
    encryption_tests.cpp
    hosts_tests.cpp
    inputcommand_tests.cpp
    pinfo_tests.cpp
    precord_tests.cpp
    support/dmfc_builders.cpp
    support/dmfc_fakes.cpp
    support/dmfc_test_support.cpp
    support/hosts_file.cpp
    support/ice_util.cpp
    support/pinfo_util.cpp
  LIBRARIES cfile dmfc
  INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/..
)
