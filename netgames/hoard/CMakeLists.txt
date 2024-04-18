SET (HEADERS hoardaux.h Hoard.h hoardstr.h)
SET (CPPS hoard.cpp hoard_ui.cpp )

SET (NETGAME_MODULE hoard)

ADD_LIBRARY(${NETGAME_MODULE} SHARED ${CPPS} ${HEADERS})
set_target_properties(${NETGAME_MODULE} PROPERTIES PREFIX "")
set_target_properties(${NETGAME_MODULE} PROPERTIES SUFFIX ".d3m")

target_link_libraries(${NETGAME_MODULE} dmfc)
