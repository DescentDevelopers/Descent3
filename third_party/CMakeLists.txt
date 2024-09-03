set(CMAKE_FOLDER "third_party")

add_subdirectory(libacm)
add_subdirectory(stb)

if(USE_EXTERNAL_PLOG)
  find_package(plog REQUIRED)
else()
  if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/plog/CMakeLists.txt)
    message(FATAL_ERROR "plog third-party directory could not be found. "
      "Please run 'submodule update --init --recursive' in the source directory.")
  endif()
  add_subdirectory(plog)
endif()
