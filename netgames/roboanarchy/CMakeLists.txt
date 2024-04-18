SET (HEADERS roboAnarchy.h roboanarchystr.h)
SET (CPPS roboanarchy.cpp)

SET (NETGAME_MODULE roboanarchy)

ADD_LIBRARY(${NETGAME_MODULE} SHARED ${CPPS} ${HEADERS})
set_target_properties(${NETGAME_MODULE} PROPERTIES PREFIX "")
set_target_properties(${NETGAME_MODULE} PROPERTIES SUFFIX ".d3m")

target_link_libraries(${NETGAME_MODULE} dmfc)
