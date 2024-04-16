SET (HEADERS Entropy.h Entropystr.h EntropyAux.h)
SET (CPPS EntropyBase.cpp EntropyPackets.cpp EntropyRoom.cpp)

SET (NETGAME_MODULE entropy)

ADD_LIBRARY(${NETGAME_MODULE} SHARED ${CPPS} ${HEADERS})
set_target_properties(${NETGAME_MODULE} PROPERTIES PREFIX "")
set_target_properties(${NETGAME_MODULE} PROPERTIES SUFFIX ".d3m")

target_link_libraries(${NETGAME_MODULE} dmfc)
install(TARGETS ${NETGAME_MODULE} DESTINATION "${D3_GAMEDIR}netgames/")