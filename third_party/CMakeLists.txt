set(CMAKE_FOLDER "third_party")

add_subdirectory(libacm)
add_subdirectory(stb)

if (NOT DEFINED USING_VCPKG) # VCPKG has ports for SDL3 and plog
  message("USING BUNDLED SDL")
  if(NOT USE_EXTERNAL_PLOG)
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/plog/CMakeLists.txt)
      message(FATAL_ERROR "plog third-party directory could not be found. "
        "Please run 'git submodule update --init --recursive' in the source directory.")
    endif()
    add_subdirectory(plog)
  endif()

  if (NOT USE_EXTERNAL_SDL)
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/SDL/CMakeLists.txt)
    message(FATAL_ERROR "SDL third-party directory could not be found. "
      "Please run 'git submodule update --init --recursive' in the source directory.")
    endif()
    add_subdirectory(SDL)
  endif()
endif()