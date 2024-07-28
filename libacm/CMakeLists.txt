set(CPPS
  adecode.cpp
  aencode.cpp
)

# these are the relevant source files from upstream libacm (https://github.com/markokr/libacm/)
set(LIB_SRC
  decode.c
  libacm.h
)

add_library(libacm STATIC ${CPPS} ${LIB_SRC})
target_include_directories(libacm PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/libacm
  >
)
