SET (HEADERS )
SET (CPPS
		Collide.cpp
		FindIntersection.cpp
		newstyle_fi.cpp
		physics.cpp)

ADD_LIBRARY(physics STATIC ${HEADERS} ${CPPS})