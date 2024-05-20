set(HEADERS)
set(CPPS
  musicapi.cpp)

add_library(d3music STATIC ${HEADERS} ${CPPS})
target_link_libraries(d3music PRIVATE
  music
)
