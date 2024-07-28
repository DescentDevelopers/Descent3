set(CPPS
  adecode.cpp
  aencode.cpp
  encoder.cpp
)

add_library(AudioEncode STATIC ${CPPS})
target_link_libraries(AudioEncode PRIVATE
  libacm
)
target_include_directories(AudioEncode PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/AudioEncode
  >
)
