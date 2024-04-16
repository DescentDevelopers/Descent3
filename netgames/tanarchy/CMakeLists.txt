SET (HEADERS tanarchystr.h tanDMFC.h)
SET (CPPS tanarchy.cpp)

SET (NETGAME_MODULE tanarchy)

ADD_LIBRARY(${NETGAME_MODULE} SHARED ${CPPS} ${HEADERS})
set_target_properties(${NETGAME_MODULE} PROPERTIES PREFIX "")
set_target_properties(${NETGAME_MODULE} PROPERTIES SUFFIX ".d3m")

target_link_libraries(${NETGAME_MODULE} dmfc)
install(TARGETS ${NETGAME_MODULE} DESTINATION "${D3_GAMEDIR}netgames/")
