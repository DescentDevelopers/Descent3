d3_test_sources(dd_grwin32_tests TO d3_tests
  SOURCES ddgr_dx_tests.cpp ddgr_gdi_tests.cpp support/dx_surface.cpp support/generic_surface.cpp
    support/locked_pixels.cpp
  LIBRARIES dd_grwin32 win32 ddebug
  INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/..
  # application.h selects the Win32 application class only when DX_APP is defined.
  DEFINITIONS DX_APP
)
