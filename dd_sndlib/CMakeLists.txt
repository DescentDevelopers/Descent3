set(HEADERS 
		auddev.h
		ds3dlib_internal.h
		eax.h
		eax2.h
		ia3dapi.h
		Ia3dutil.h
		vmanpset.h)
set(CPPS
		aureal3d.cpp
		ddsoundload.cpp
		Ds3dlib.cpp
		dsound3d.cpp
		eax.cpp
		geometry.cpp
		ssl_lib.cpp)

add_library(dd_sndlib STATIC ${HEADERS} ${CPPS})