SET (HEADERS )
SET (CPPS
		BitIO.cpp
		CZip.cpp
		HuffmanAdapt.cpp
		HuffmanBasic.cpp)

ADD_LIBRARY(czip STATIC ${HEADERS} ${CPPS})