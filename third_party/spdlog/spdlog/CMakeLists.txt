find_package(Threads REQUIRED)
add_library(spdlog_header_only INTERFACE)

target_include_directories(
    spdlog_header_only ${SPDLOG_INCLUDES_LEVEL} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(spdlog_header_only INTERFACE Threads::Threads)