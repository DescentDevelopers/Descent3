SET (HEADERS anarchystr.h Anarchy.h)
SET (CPPS anarchy.cpp)

SET (NETGAME_MODULE anarchy)

ADD_LIBRARY(${NETGAME_MODULE} SHARED ${CPPS} ${HEADERS})
set_target_properties(${NETGAME_MODULE} PROPERTIES PREFIX "")
set_target_properties(${NETGAME_MODULE} PROPERTIES SUFFIX ".d3m")

target_link_libraries(${NETGAME_MODULE} dmfc)
