cmake_path(SET GENERATED_HEADERS NORMALIZE ${CMAKE_CURRENT_BINARY_DIR}/generated)
cmake_path(SET SHADERS_H NORMALIZE ${GENERATED_HEADERS}/shaders.h)
set(SHADERS
  shaders/fragment.glsl
  shaders/vertex.glsl
  shaders/version_300_es.glsl
  shaders/version_150_core.glsl
)
add_custom_command(
  OUTPUT ${SHADERS_H}
  COMMAND ${CMAKE_COMMAND} -Dshaders_h=${SHADERS_H} -P ../cmake/shaders.cmake -- ${SHADERS}
  DEPENDS ../cmake/shaders.cmake ${SHADERS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(HEADERS
  dyna_gl.h
  HardwareInternal.h
  ShaderProgram.h
  ${SHADERS_H}
)
set(CPPS
  HardwareClipper.cpp
  HardwareDraw.cpp
  HardwareGlobalVars.cpp
  HardwareInstance.cpp
  HardwareOpenGL.cpp
  HardwareBaseGPU.cpp
  HardwarePoints.cpp
  HardwareSetup.cpp
  HardwareTransforms.cpp
  lnxscreenmode.cpp
)

add_library(renderer STATIC ${HEADERS} ${CPPS})
target_include_directories(renderer PRIVATE ${GENERATED_HEADERS})
target_link_libraries(renderer PRIVATE
  glm::glm
  SDL2::SDL2
  bitmap
  ddebug
  ddio
  logger
  mem
  misc
  module
  rtperformance
)
