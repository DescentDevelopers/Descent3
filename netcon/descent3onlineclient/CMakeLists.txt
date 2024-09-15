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
  misc
  ui
  $<$<PLATFORM_ID:Windows>:ws2_32>
)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set_target_properties(Descent3_Online_TCP_IP PROPERTIES SUFFIX ".dylib")
endif()

add_custom_target(Descent3_Online_TCP_IP_Hog
  COMMAND $<TARGET_FILE:HogMaker>
  "$<TARGET_FILE_DIR:Descent3>/online/Descent3 Online.d3c"
  "${CMAKE_SOURCE_DIR}/netcon/descent3onlineclient/d3online.d3c.txt"
  "${CMAKE_SOURCE_DIR}/scripts/data/fullhog/"
  "$<TARGET_FILE_DIR:Descent3_Online_TCP_IP>"

  DEPENDS Descent3_Online_TCP_IP HogMaker
  COMMENT "Generate 'Descent3 Online.d3c'"
)
install(FILES "$<TARGET_FILE_DIR:Descent3>/online/Descent3 Online.d3c" DESTINATION ${CMAKE_INSTALL_DATADIR}/online)
