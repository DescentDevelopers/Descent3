set(HEADERS
  UIlib.h
  ui.h
  uidraw.h
  uires.h
  uisys.h)
set(CPPS
  UIButton.cpp
  UICombo.cpp
  UIConsole.cpp
  UIDraw.cpp
  UIEdit.cpp
  UIGadget.cpp
  UIGroup.cpp
  UIHotspot.cpp
  UIListBox.cpp
  UIObject.cpp
  UIRes.cpp
  UISlider.cpp
  UIStatic.cpp
  UISystem.cpp
  UIWindow.cpp
)

add_library(ui STATIC ${HEADERS} ${CPPS})
target_link_libraries(ui PRIVATE
  ddio
  grtext
  mem
  misc
  plog::plog
)
target_include_directories(ui PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/ui
  >
)
