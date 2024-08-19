set(CMAKE_FOLDER "third_party")

add_subdirectory(libacm)
add_subdirectory(stb)

if(USE_EXTERNAL_PLOG)
  find_package(plog REQUIRED)
else()
  add_subdirectory(plog)
endif()
