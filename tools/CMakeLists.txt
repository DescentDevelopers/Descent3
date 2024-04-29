#cmake_minimum_required(VERSION 3.28)

set(HEADERS
  HogEdit/string_hash.h
  HogEdit/HogFormat.h
  HogEdit/IOOps.h)

set(CPPS
  HogEdit/HogFormat.cpp
  HogEdit/HogEdit.cpp)

project(HogEdit
  LANGUAGES C CXX
  VERSION 0.1.0)

add_custom_command(
  OUTPUT ${PROJECT_BINARY_DIR}/HogEdit/version.h ALL
  COMMAND ${CMAKE_COMMAND}
    -D SOURCE_DIR=${PROJECT_SOURCE_DIR}
    -D TARGET_DIR=${PROJECT_BINARY_DIR}
    -D TARGET_HEADER=HogEdit/version.h
    -D TEMPLATE_HEADER=../lib/d3_version.h.in
    -D MACRO_PREFIX=HOGEDIT
    -D PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    -D PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    -D PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    -P ${PROJECT_SOURCE_DIR}/../cmake/CheckGit.cmake
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)


# rebuild version.h every time
add_custom_target(get_hogedit_hash ALL
  DEPENDS ${PROJECT_BINARY_DIR}/HogEdit/version.h
)

add_executable(hogedit ${HEADERS} ${CPPS})
add_dependencies(hogedit get_hogedit_hash)
target_include_directories(hogedit PRIVATE ${PROJECT_BINARY_DIR}/HogEdit)
