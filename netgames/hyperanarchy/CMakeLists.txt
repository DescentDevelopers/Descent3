SET (HEADERS HyperAnarchy.h hyperstr.h)
SET (CPPS hyperanarchy.cpp)

SET (NETGAME_MODULE hyperanarchy)

ADD_LIBRARY(${NETGAME_MODULE} SHARED ${CPPS} ${HEADERS})
set_target_properties(${NETGAME_MODULE} PROPERTIES PREFIX "")
set_target_properties(${NETGAME_MODULE} PROPERTIES SUFFIX ".d3m")

target_link_libraries(${NETGAME_MODULE} dmfc)
