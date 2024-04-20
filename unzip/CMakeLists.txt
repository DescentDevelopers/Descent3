set(HEADERS infblock.h
		infcodes.h
		inffast.h
		inffixed.h
		inftrees.h
		infutil.h
		zconf.h
		zlib.h
		zutil.h )
set(CPPS
		unzip.cpp
		adler32.c
		infblock.c
		infcodes.c
		inffast.c
		inflate.c
		inftrees.c
		infutil.c
		)

add_library(unzip STATIC ${HEADERS} ${CPPS})