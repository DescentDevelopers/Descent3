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
  macos_sign(Parallax_Online)
endif()

add_custom_target(Parallax_Online_Hog
  COMMAND ${HogMakerBin}
  "${D3_GENERATED_FILES_OUTPUT_DIRECTORY}/online/Parallax Online.d3c"
  "${CMAKE_SOURCE_DIR}/netcon/mtclient/Parallax_Online.d3c.txt"
  "${CMAKE_SOURCE_DIR}/scripts/data/fullhog/"
  "$<TARGET_FILE_DIR:Parallax_Online>"
  DEPENDS Parallax_Online HogMaker
  COMMENT "Generate 'Parallax Online.d3c'"
)

install(
        FILES "${D3_GENERATED_FILES_OUTPUT_DIRECTORY}/online/Parallax Online.d3c"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/online
)
