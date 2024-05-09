find_package(Threads REQUIRED)
add_library(spdlog_header INTERFACE)

target_include_directories(
    spdlog_header ${SPDLOG_INCLUDES_LEVEL} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(spdlog_header INTERFACE Threads::Threads)
