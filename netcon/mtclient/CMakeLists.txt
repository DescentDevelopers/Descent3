set(CPPS
  chat_api.cpp
  mt_net.cpp
  mtclient.cpp
  mtgametrack.cpp
  mtpilottracker.cpp
)

add_library(Parallax_Online MODULE ${CPPS})
set_target_properties(Parallax_Online
  PROPERTIES
  PREFIX ""
  CXX_VISIBILITY_PRESET "hidden"
  OUTPUT_NAME "Parallax Online"
  LIBRARY_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:Descent3>/online"
  SUFFIX "d3c"
)
#set_target_properties(Parallax_Online PROPERTIES CXX_VISIBILITY_PRESET "hidden")
#set_target_properties(Parallax_Online PROPERTIES OUTPUT_NAME "Parallax Online")
target_link_libraries(Parallax_Online PRIVATE
  ddio
#  inetfile
#  $<$<PLATFORM_ID:Windows>:
#  ws2_32
#  >
)

#add_custom_target(Direct_TCP_IP_Hog
#  COMMAND $<TARGET_FILE:HogMaker>
#  "$<TARGET_FILE_DIR:Descent3>/online/Direct TCP~IP.d3c"
#  "${CMAKE_SOURCE_DIR}/netcon/lanclient/TCP_IP.d3c.txt"
#  "${CMAKE_SOURCE_DIR}/scripts/data/fullhog/"
#  "$<TARGET_FILE_DIR:Direct_TCP_IP>"

#  DEPENDS Direct_TCP_IP HogMaker
#  COMMENT "Generate 'Direct TCP~IP.d3c'"
#)
install(FILES "$<TARGET_FILE_DIR:Descent3>/online/Parallax Online.d3c" DESTINATION ${CMAKE_INSTALL_DATADIR}/online)
