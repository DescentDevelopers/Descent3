set(CPPS unzip.cpp)

add_library(unzip STATIC ${HEADERS} ${CPPS})

target_link_libraries(unzip PRIVATE ZLIB::ZLIB)
target_include_directories(unzip PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/unzip
  >
)
