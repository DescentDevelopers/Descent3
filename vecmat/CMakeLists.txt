set(HEADERS)
set(CPPS
  vector.cpp)

add_library(vecmat STATIC ${HEADERS} ${CPPS})
target_link_libraries(vecmat PRIVATE
  ddebug
  fix
  misc
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
