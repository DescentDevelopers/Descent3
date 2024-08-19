set(HEADERS
  cfile.h
  hogfile.h
  inffile.h)
set(CPPS
  cfile.cpp
  hogfile.cpp
  inffile.cpp
)

add_library(cfile STATIC ${HEADERS} ${CPPS})
target_link_libraries(cfile PRIVATE
  ddio
  mem
  misc
  plog::plog
)
target_include_directories(cfile PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/cfile
  >
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
