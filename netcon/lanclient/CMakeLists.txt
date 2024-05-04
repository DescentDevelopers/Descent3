set(HEADERS lanclient.h)
set(CPPS lanclient.cpp)

set(NETGAME_MODULE "TCP_IP")

add_library(${NETGAME_MODULE} MODULE ${CPPS} ${HEADERS})
set_target_properties(${NETGAME_MODULE} PROPERTIES PREFIX "")
set_target_properties(${NETGAME_MODULE} PROPERTIES SUFFIX ".d3c")
target_link_libraries(${NETGAME_MODULE} inetfile)
