set(CPPS
  aencode.cpp
  adecode.cpp
)

add_library(libacm STATIC ${HEADERS} ${CPPS})
target_link_libraries(libacm PRIVATE libacm-ext)
