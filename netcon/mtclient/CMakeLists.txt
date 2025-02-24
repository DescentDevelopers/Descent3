set(HEADERS
  chat_api.h
  mt_net.h
  mtclient.h
  mtgametrack.h
  mtpilottrack.h
  mtstrings.h)
set(CPPS
  chat_api.cpp
  mt_net.cpp
  mtclient.cpp
  mtgametrack.cpp
  mtpilottracker.cpp
)

add_library(Parallax_Online MODULE ${HEADERS} ${CPPS})
set_target_properties(Parallax_Online PROPERTIES PREFIX "")
set_target_properties(Parallax_Online PROPERTIES CXX_VISIBILITY_PRESET "hidden")
set_target_properties(Parallax_Online PROPERTIES OUTPUT_NAME "Parallax Online")
target_link_libraries(Parallax_Online PRIVATE
  ddio
  inetfile
  misc
  module
  ui
  $<$<PLATFORM_ID:Windows>:ws2_32>
  SDL3::SDL3
)
target_include_directories(Parallax_Online PRIVATE ${SDL3_INCLUDE_DIRS})
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set_target_properties(Parallax_Online PROPERTIES SUFFIX ".dylib")
  if(DEFINED CODESIGN_IDENTITY AND NOT "${CODESIGN_IDENTITY}" STREQUAL "")
    message(STATUS "Code signing Parallax_Online")
    add_custom_command(TARGET Parallax_Online POST_BUILD
            COMMAND codesign --verbose --sign "${CODESIGN_IDENTITY}" --force --timestamp --deep -o runtime $<TARGET_FILE:Parallax_Online>)
  endif()
endif()

include(HogMaker)
MakeHog(
  TARGET Parallax_Online_Hog
  OUTPUT "${D3_GENERATED_FILES_OUTPUT_DIRECTORY}/online/Parallax Online.d3c"
  INPUT_FILE "${CMAKE_SOURCE_DIR}/netcon/mtclient/Parallax_Online.d3c.txt"
  SEARCH_PATH "$<TARGET_FILE_DIR:Parallax_Online>" "${CMAKE_SOURCE_DIR}/scripts/data/fullhog/"
  DEPENDS Parallax_Online
)

install(
  FILES "${D3_GENERATED_FILES_OUTPUT_DIRECTORY}/online/Parallax Online.d3c"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/online
)
