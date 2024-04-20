set(HEADERS 
		dyna_gl.h
		HardwareInternal.h
		RendererConfig.h
		SoftwareInternal.h)
set(CPPS
		HardwareClipper.cpp
		HardwareDraw.cpp
		HardwareGlobalVars.cpp
		HardwareInstance.cpp
		HardwareOpenGL.cpp
		HardwarePoints.cpp
		HardwareSetup.cpp
		HardwareTransforms.cpp
	
		SoftwareClipper.cpp
		SoftwareDraw.cpp
		SoftwareGlobalVars.cpp
		SoftwareInstance.cpp
		SoftwareOpenGL.cpp
		SoftwareOpenGLMeshBuilder.cpp
		SoftwarePoints.cpp
		SoftwareSetup.cpp)

if(UNIX)
  set (CPPS ${CPPS} lnxscreenmode.cpp)
endif()

# These are excluded.
#opengl.cpp
#renderer.cpp
#Direct3D.cpp

add_library(renderer STATIC ${HEADERS} ${CPPS})