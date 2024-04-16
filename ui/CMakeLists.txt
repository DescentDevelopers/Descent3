SET (HEADERS UIlib.h )
SET (CPPS
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
		UIWindow.cpp)

ADD_LIBRARY(ui STATIC ${HEADERS} ${CPPS})