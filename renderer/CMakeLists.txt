set(HEADERS
  dyna_gl.h
  HardwareInternal.h
  rend_d3d.h
  renderer.h
  rend_opengl.h
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
)

if(UNIX)
  set(HEADERS ${HEADERS} lnxscreenmode.h)
  set(CPPS ${CPPS} lnxscreenmode.cpp)
endif()

# These are excluded.
#opengl.cpp
#renderer.cpp
#Direct3D.cpp

add_library(renderer STATIC ${HEADERS} ${CPPS})
target_link_libraries(renderer PRIVATE
  bitmap
  ddio
)

target_include_directories(renderer PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/rtperformance
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/module
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/win32
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/linux
  >
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/2dlib
  >
)
