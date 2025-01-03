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

set(DEFAULT_ADDITIONAL_DIRS "{}" CACHE STRING "A list of directories that Descent 3 will use as read-only base directories.")
set(DBD_CPP ${CMAKE_CURRENT_BINARY_DIR}/default_base_directories.cpp)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/default_base_directories.cpp.in
  ${DBD_CPP}
  @ONLY
)

add_library(cfile STATIC ${HEADERS} ${CPPS} ${DBD_CPP})
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
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
