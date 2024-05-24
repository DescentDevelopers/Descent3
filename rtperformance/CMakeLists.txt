set(HEADERS)
set(CPPS
  rtperformance.cpp)

add_library(rtperformance STATIC ${HEADERS} ${CPPS})
target_link_libraries(rtperformance PRIVATE
  ddio
)
