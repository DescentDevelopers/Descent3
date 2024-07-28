set(CPPS
  rtperformance.cpp)

add_library(rtperformance STATIC ${CPPS})
target_link_libraries(rtperformance PRIVATE
  ddio
)
target_include_directories(rtperformance PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/rtperformance
  >
)
