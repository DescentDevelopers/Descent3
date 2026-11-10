# The dallas library carries the script modules' own vector math (osiris_vector.cpp), which defines the same vm_*
# functions as the vecmat library the rest of the tests link, so its tests need an executable of their own.
d3_test_executable(dallas_tests)
d3_test_sources(dallas_test_sources TO dallas_tests
  SOURCES dallas_uservar_tests.cpp dallas_vector_tests.cpp support/dallas_fake_engine.cpp
    support/dallas_vector_helpers.cpp
  LIBRARIES dallas
  INCLUDES ${PROJECT_SOURCE_DIR}/scripts
)

include(${CMAKE_CURRENT_SOURCE_DIR}/aiscripts_tests.cmake)
