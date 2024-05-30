set(HEADERS
  ../lib/movie.h
  ../lib/mvelibw.h
  ../lib/pserror.h
  ../lib/renderer.h
  ../lib/application.h
  ../lib/grtext.h
  ../lib/mem.h
  ../lib/bitmap.h
  ../Descent3/gamefont.h
  ../Descent3/game.h
)
set(CPPS
  d3movie.cpp)

add_library(movie STATIC ${HEADERS} ${CPPS})
target_link_libraries(movie PRIVATE
  ddio
  libmve
)
