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
  SDL3::SDL3  
)
target_include_directories(Direct_TCP_IP PRIVATE ${SDL3_INCLUDE_DIRS})
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set_target_properties(Direct_TCP_IP PROPERTIES SUFFIX ".dylib")
  if(DEFINED CODESIGN_IDENTITY AND NOT "${CODESIGN_IDENTITY}" STREQUAL "")
    message(STATUS "Code signing Direct_TCP_IP")
    add_custom_command(TARGET Direct_TCP_IP POST_BUILD
            COMMAND codesign --verbose --sign "${CODESIGN_IDENTITY}" --force --timestamp --deep -o runtime $<TARGET_FILE:Direct_TCP_IP>)
  endif()
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
