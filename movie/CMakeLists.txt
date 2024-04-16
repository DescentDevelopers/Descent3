SET (HEADERS )
SET (CPPS
		d3movie.cpp)

ADD_LIBRARY(movie STATIC ${HEADERS} ${CPPS})
target_link_libraries(movie libmve)