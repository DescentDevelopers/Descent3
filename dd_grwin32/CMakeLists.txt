set(HEADERS
  ddgrWin32.h
  ddgrWin32DX.h
  ddgrWin32GDI.h
)
set(CPPS
  ddgrWin32API.cpp
  ddgrWin32DX.cpp
  ddgrWin32GDI.cpp
  ddgrWin32Init.cpp
)

add_library(dd_grwin32 STATIC ${HEADERS} ${CPPS})
target_link_libraries(dd_grwin32 PRIVATE
  2dlib
  ddebug
  logger
  ${DDRAW_LIBRARY}
)
target_compile_definitions(dd_grwin32 PRIVATE DX_APP)
target_include_directories(dd_grwin32 PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/dd_grwin32
  >
)
