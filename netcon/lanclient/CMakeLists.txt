set(HEADERS
  lanclient.h
  lanstrings.h)
set(CPPS lanclient.cpp)

add_library(Direct_TCP_IP MODULE ${HEADERS} ${CPPS})
set_target_properties(Direct_TCP_IP PROPERTIES PREFIX "")
set_target_properties(Direct_TCP_IP PROPERTIES CXX_VISIBILITY_PRESET "hidden")
set_target_properties(Direct_TCP_IP PROPERTIES OUTPUT_NAME "Direct TCP~IP")
target_link_libraries(Direct_TCP_IP PRIVATE
  ddio
  inetfile
  misc
  module
  ui
  $<$<PLATFORM_ID:Windows>:ws2_32>
)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set_target_properties(Direct_TCP_IP PROPERTIES SUFFIX ".dylib")
endif()

include(HogMaker)
MakeHog(
  TARGET Direct_TCP_IP_Hog
  OUTPUT "${D3_GENERATED_FILES_OUTPUT_DIRECTORY}/online/Direct TCP~IP.d3c"
  INPUT_FILE "${CMAKE_SOURCE_DIR}/netcon/lanclient/TCP_IP.d3c.txt"
  SEARCH_PATH "$<TARGET_FILE_DIR:Direct_TCP_IP>" "${CMAKE_SOURCE_DIR}/scripts/data/fullhog/"
  DEPENDS Direct_TCP_IP
)

install(
  FILES "${D3_GENERATED_FILES_OUTPUT_DIRECTORY}/online/Direct TCP~IP.d3c"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/online
)
