set(CPPS
  newstyle.cpp
  polymodel.cpp
)

add_library(model STATIC ${CPPS})
target_link_libraries(model PRIVATE
  cfile
  mem
  misc
)
target_include_directories(model PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/model
  >
)
