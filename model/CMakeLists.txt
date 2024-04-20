set(HEADERS )
set(CPPS
		newstyle.cpp
		polymodel.cpp)

add_library(model STATIC ${HEADERS} ${CPPS})