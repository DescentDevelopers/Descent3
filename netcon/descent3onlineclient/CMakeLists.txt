set(HEADERS
  chat_api.h
  odtclient.h
  odtstrings.h
  dip_gametrack.h
  )
set(CPPS
  chat_api.cpp
  odtclient.cpp
  dip_gametrack.cpp
)

add_library(Descent3_Online_TCP_IP MODULE ${HEADERS} ${CPPS})
set_target_properties(Descent3_Online_TCP_IP PROPERTIES PREFIX "")
set_target_properties(Descent3_Online_TCP_IP PROPERTIES CXX_VISIBILITY_PRESET "hidden")
set_target_properties(Descent3_Online_TCP_IP PROPERTIES OUTPUT_NAME "Descent3 Online")
target_link_libraries(Descent3_Online_TCP_IP PRIVATE
  ddio
  inetfile
  httplib::httplib
  misc
  module
  ui
  $<$<PLATFORM_ID:Windows>:ws2_32>
  SDL3::SDL3
)
target_include_directories(Descent3_Online_TCP_IP PRIVATE ${SDL3_INCLUDE_DIRS})
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set_target_properties(Descent3_Online_TCP_IP PROPERTIES SUFFIX ".dylib")
  if(DEFINED CODESIGN_IDENTITY AND NOT "${CODESIGN_IDENTITY}" STREQUAL "")
    message(STATUS "Code signing Descent3_Online_TCP_IP")
    add_custom_command(TARGET Descent3_Online_TCP_IP POST_BUILD
            COMMAND codesign --verbose --sign "${CODESIGN_IDENTITY}" --force --timestamp --deep -o runtime $<TARGET_FILE:Descent3_Online_TCP_IP>)
  endif()
endif()

include(HogMaker)
MakeHog(
  TARGET Descent3_Online_TCP_IP_Hog
  OUTPUT "${D3_GENERATED_FILES_OUTPUT_DIRECTORY}/online/Descent3 Online.d3c"
  INPUT_FILE "${CMAKE_SOURCE_DIR}/netcon/descent3onlineclient/d3online.d3c.txt"
  SEARCH_PATH "$<TARGET_FILE_DIR:Descent3_Online_TCP_IP>" "${CMAKE_SOURCE_DIR}/scripts/data/fullhog/"
  DEPENDS Descent3_Online_TCP_IP
)

install(
  FILES "${D3_GENERATED_FILES_OUTPUT_DIRECTORY}/online/Descent3 Online.d3c"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/online
)
