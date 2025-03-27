set(HEADERS
  cfile.h
  default_base_directories.h
  hogfile.h
  inffile.h)
set(CPPS
  cfile.cpp
  hogfile.cpp
  inffile.cpp
)

set(DEFAULT_ADDITIONAL_DIRS "" CACHE STRING "A list of directories that Descent 3 will use as read-only base directories.")
string(REPLACE ";" "\",\n  \"" DEFAULT_ADDITIONAL_DIRS_CPP_EXPR "${DEFAULT_ADDITIONAL_DIRS}")
string(PREPEND DEFAULT_ADDITIONAL_DIRS_CPP_EXPR "\"")
string(APPEND DEFAULT_ADDITIONAL_DIRS_CPP_EXPR "\"")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/default_base_directories.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/default_base_directories.h
  @ONLY
)

add_library(cfile STATIC ${HEADERS} ${CPPS})
target_link_libraries(cfile PRIVATE
  ddebug
  ddio
  logger
  mem
  misc
)
target_include_directories(cfile PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/cfile
  >
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}   # For default_base_directories.h
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
