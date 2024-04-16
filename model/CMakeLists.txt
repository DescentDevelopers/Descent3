SET (HEADERS )
SET (CPPS
		newstyle.cpp
		polymodel.cpp)

ADD_LIBRARY(model STATIC ${HEADERS} ${CPPS})