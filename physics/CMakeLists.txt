set(HEADERS
  ../Descent3/object.h
  ../lib/PHYSICS.H
  ../lib/collide.h
  ../lib/findintersection.h
  ../lib/vecmat.h
  ../Descent3/game.h
  ../Descent3/terrain.h
  ../Descent3/descent.h
  ../Descent3/weapon.h
  ../lib/polymodel.h
  ../Descent3/fireball.h
  ../Descent3/damage.h
  ../Descent3/gameevent.h
  ../sndlib/hlsoundlib.h
  ../sndlib/soundload.h
  ../Descent3/viseffect.h
  ../Descent3/multi.h
  ../Descent3/attach.h
  ../Descent3/D3ForceFeedback.h
  ../Descent3/player.h
  ../lib/demofile.h
  ../Descent3/vibeinterface.h
)
set(CPPS
  Collide.cpp
  FindIntersection.cpp
  newstyle_fi.cpp
  physics.cpp)

add_library(physics STATIC ${HEADERS} ${CPPS})
target_link_libraries(physics PRIVATE
  ddio
  sndlib
)
