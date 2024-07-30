add_library(libacm STATIC decode.c libacm.h)
target_include_directories(libacm PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/libacm
  >
)
