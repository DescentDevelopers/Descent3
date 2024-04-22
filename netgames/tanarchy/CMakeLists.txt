set(HEADERS tanarchystr.h tanDMFC.h)
set(CPPS tanarchy.cpp)

set(NETGAME_MODULE tanarchy)

add_library(${NETGAME_MODULE} MODULE ${CPPS} ${HEADERS})
set_target_properties(${NETGAME_MODULE} PROPERTIES PREFIX "")
set_target_properties(${NETGAME_MODULE} PROPERTIES SUFFIX ".d3m")

target_link_libraries(${NETGAME_MODULE} dmfc)
