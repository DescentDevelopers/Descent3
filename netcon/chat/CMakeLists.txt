# The IRC chat client shared by the online lobby modules (Descent3 Online and Parallax Online). It is linked into
# each module, so it is built position-independent and keeps the modules' hidden symbol visibility.
add_library(netcon_chat STATIC chat_api.cpp chat_api.h)
set_target_properties(netcon_chat PROPERTIES POSITION_INDEPENDENT_CODE ON CXX_VISIBILITY_PRESET "hidden")
target_include_directories(netcon_chat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(netcon_chat PRIVATE
  logger
  $<$<PLATFORM_ID:Windows>:ws2_32>
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
