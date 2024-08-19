set(HEADERS 
  polymodel.h)
set(CPPS
  newstyle.cpp
  polymodel.cpp
)

add_library(model STATIC ${HEADERS} ${CPPS})
target_link_libraries(model PRIVATE
  cfile
  mem
  misc
  physics
  plog::plog
)
target_include_directories(model PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/model
  >
)
