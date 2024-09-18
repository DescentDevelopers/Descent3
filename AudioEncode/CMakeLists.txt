set(HEADERS
  adecode.h
  aencode.h
  audio_encode.h)
set(CPPS
  adecode.cpp
  aencode.cpp
  encoder.cpp
)

add_library(AudioEncode STATIC ${HEADERS} ${CPPS})
target_link_libraries(AudioEncode PRIVATE
  libacm
  logger
)
target_include_directories(AudioEncode PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/AudioEncode
  >
)
