d3_test_sources(bitmap_tests TO d3_tests
  SOURCES
    bitmain_tests.cpp
    image_loader_tests.cpp
    newbitmap_lightmap_tests.cpp
    support/image_builders.cpp
    support/image_loading.cpp
    support/test_support.cpp
  LIBRARIES bitmap cfile d3_test_game ddebug ddio logger mem misc SDL3::SDL3
)
