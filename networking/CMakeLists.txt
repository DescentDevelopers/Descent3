set(HEADERS
  ../lib/win/DirectX/dplay.h
  ../lib/win/DirectX/dplobby.h
  ../Descent3/descent.h
  ../lib/appdatabase.h
  ../lib/pstypes.h
  ../lib/pserror.h
  ../lib/networking.h
  ../lib/mem.h
  ../Descent3/game.h
  ../Descent3/args.h
)
set(CPPS
  networking.cpp)

set(PLATFORMCPPS)

if(WIN32)
  set(PLATFORMCPPS "directplay.cpp")
endif()

add_library(networking STATIC ${HEADERS} ${CPPS} ${PLATFORMCPPS})
target_link_libraries(networking PRIVATE
  ddio
)
