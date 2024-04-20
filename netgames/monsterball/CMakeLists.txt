SET (HEADERS monsterball.h monsterstr.h)
SET (CPPS monsterball.cpp)

SET (NETGAME_MODULE monsterball)

ADD_LIBRARY(${NETGAME_MODULE} SHARED ${CPPS} ${HEADERS})
set_target_properties(${NETGAME_MODULE} PROPERTIES PREFIX "")
set_target_properties(${NETGAME_MODULE} PROPERTIES SUFFIX ".d3m")

target_link_libraries(${NETGAME_MODULE} dmfc)
