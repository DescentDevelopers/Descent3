d3_test_sources(unzip_tests TO d3_tests
  SOURCES unzip_tests.cpp support/zip_builder.cpp support/zip_reader.cpp
  LIBRARIES unzip ZLIB::ZLIB
)
