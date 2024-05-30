set(HEADERS
  ../Descent3/objinfo.h
  ../lib/polymodel.h
  ../lib/pserror.h
  ../lib/3d.h
  ../lib/bitmap.h
  ../lib/renderer.h
  ../lib/manage.h
  ../Descent3/gametexture.h
  ../Descent3/lighting.h
  ../Descent3/game.h
  ../Descent3/robotfire.h
  ../lib/mem.h
  ../Descent3/gamesequence.h
  ../Descent3/render.h
  ../Descent3/fireball.h
  ../Descent3/lightmap_info.h
  ../lib/lightmap.h
  ../lib/findintersection.h
)

set(CPPS
  newstyle.cpp
  polymodel.cpp)

add_library(model STATIC ${HEADERS} ${CPPS})
target_link_libraries(model PRIVATE
  ddio
)
