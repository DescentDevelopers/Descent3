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
set(DEFAULT_ADDITIONAL_DIRS_CPP_EXPR "{")
foreach(CURRENT_PATH IN LISTS DEFAULT_ADDITIONAL_DIRS)
  # This is an implementation of algorithm 2 from this blog post [1]. The only
  # real difference between the code in that blog post and the code in this
  # file is that the code in this file percent-decodes [2] the input string.
  # Percent-decoding the input string is a workaround for the fact that you
  # can’t include certain sequences of characters in CMake lists [3].
  #
  # [1]: <https://jasonyundt.website/posts/string-to-cpp-literal>
  # [2]: <https://url.spec.whatwg.org/#percent-encoded-bytes>
  # [3]: <https://gitlab.kitware.com/cmake/cmake/-/issues/23315#note_1155981>
  string(APPEND DEFAULT_ADDITIONAL_DIRS_CPP_EXPR "\n  u8\"")
  string(LENGTH "${CURRENT_PATH}" CURRENT_PATH_LENGTH)
  set(I "0")
  while(I LESS "${CURRENT_PATH_LENGTH}")
    string(SUBSTRING "${CURRENT_PATH}" "${I}" 1 CURRENT_BYTE)
    if(CURRENT_BYTE STREQUAL "%")
      math(EXPR START "${I} + 1")
      string(SUBSTRING "${CURRENT_PATH}" "${START}" 2 HEX_VALUE)
      # The next two bytes have already been handled, so skip them.
      math(EXPR I "${I} + 2")
    else()
      string(HEX "${CURRENT_BYTE}" HEX_VALUE)
    endif()
    string(APPEND DEFAULT_ADDITIONAL_DIRS_CPP_EXPR "\\x${HEX_VALUE}")
    math(EXPR I "${I} + 1")
  endwhile()
  string(APPEND DEFAULT_ADDITIONAL_DIRS_CPP_EXPR "\",\n")
endforeach()
string(APPEND DEFAULT_ADDITIONAL_DIRS_CPP_EXPR "}")

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
