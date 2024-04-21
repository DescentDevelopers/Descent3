set(HEADERS)
set(CPPS
  aencode.cpp
  libacm.cpp)

# these are the relevant source files from upstream libacm (https://github.com/markokr/libacm/)
set(LIB_SRC
  decode.c
  util.c
  libacm.h)

add_library(libacm STATIC ${HEADERS} ${CPPS} ${LIB_SRC})
