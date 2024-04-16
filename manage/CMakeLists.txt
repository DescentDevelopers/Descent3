SET (HEADERS doorpage.h
		gamefilepage.h
		genericpage.h
		megapage.h
		powerpage.h
		robotpage.h
		shippage.h
		soundpage.h
		texpage.h
		weaponpage.h )
SET (CPPS
		doorpage.cpp
		gamefilepage.cpp
		generic.cpp
		manage.cpp
		megapage.cpp
		pagelock.cpp
		shippage.cpp
		soundpage.cpp
		texpage.cpp
		weaponpage.cpp)

ADD_LIBRARY(manage STATIC ${HEADERS} ${CPPS})