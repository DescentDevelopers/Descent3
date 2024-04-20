set(HEADERS)
set(CPPS
		ddio.cpp
		key.cpp)
add_library(ddio_common STATIC ${HEADERS} ${CPPS})