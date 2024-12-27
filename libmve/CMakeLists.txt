set(HEADERS
  decoders.h
  movie_sound.h
  mve_audio.h
  mvelib.h
  sound_interface.h)
set(CPPS
  decoder8.cpp
  decoder16.cpp
  movie_sound.cpp
  mve_audio.cpp
  mvelib.cpp
  mveplay.cpp
)

add_library(libmve STATIC ${HEADERS} ${CPPS})
target_link_libraries(libmve PRIVATE
  ddio
  SDL3::SDL3
)
target_include_directories(libmve PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/libmve
  >
)
