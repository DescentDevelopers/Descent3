set(CPPS
  grfont.cpp
  grtext.cpp
  textaux.cpp
)

add_library(grtext STATIC ${CPPS})
target_link_libraries(grtext PRIVATE
  ddio
  mem
  misc
)
target_include_directories(grtext PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/grtext
  >
)
