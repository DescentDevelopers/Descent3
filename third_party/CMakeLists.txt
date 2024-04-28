# libacm target

add_library(libacm-ext OBJECT
  libacm/src/decode.c
)

target_include_directories(libacm-ext PUBLIC
  $<BUILD_INTERFACE:
  ${PROJECT_SOURCE_DIR}/third_party/libacm/src
  >
)
