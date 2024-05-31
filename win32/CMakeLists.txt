set(CPPS
  winapp.cpp
  wincon.cpp
  WinController.cpp
  windata.cpp
  wintask.cpp
)

add_library(win32 STATIC ${CPPS})
add_dependencies(win32 get_git_hash)
target_include_directories(win32 PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/networking
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/renderer
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/model
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/manage
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/Descent3
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/bitmap
  >
  PRIVATE ${PROJECT_BINARY_DIR}/lib
)

target_link_libraries(win32 PRIVATE
  ddio
)
