set(HEADERS
  ddvid.h
  ddvidlib.h
)

set(CPPS
  $<$<PLATFORM_ID:Windows>:
    video_win32.cpp
    vidWin32FS.cpp
    vidWin32Win.cpp
  >
  $<$<PLATFORM_ID:Linux,Darwin>:
    video_lnx.cpp
  >
)

add_library(dd_video STATIC ${HEADERS} ${CPPS})
target_link_libraries(dd_video PRIVATE
  $<$<PLATFORM_ID:Linux,Darwin>:
    linux
  >
)

target_include_directories(dd_video PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/ddio
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/win32
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/linux
  >
)
