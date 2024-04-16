SET (HEADERS  )
SET (CPPS
		ddio.cpp
		key.cpp)

ADD_LIBRARY(ddio_common STATIC ${HEADERS} ${CPPS})