SET (HEADERS infblock.h
		infcodes.h
		inffast.h
		inffixed.h
		inftrees.h
		infutil.h
		zconf.h
		zlib.h
		zutil.h )
SET (CPPS
		unzip.cpp
		adler32.c
		infblock.c
		infcodes.c
		inffast.c
		inflate.c
		inftrees.c
		infutil.c
		)

ADD_LIBRARY(unzip STATIC ${HEADERS} ${CPPS})