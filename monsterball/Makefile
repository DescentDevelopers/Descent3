include ../../default.mak

OBJS = monsterball.o
REFS = monsterball.cpp
LIBNAME = ../../netgames/monsterball.d3m

CDLFLAGS = $(CFLAGS) -fPIC
LDLFLAGS = -shared -ldl -lm

all:	depall $(OBJS)
	gcc $(LDLFLAGS) -o $(LIBNAME) $(OBJS) /usr/lib/dmfc.so

depall:
	$(CC) -E -M $(CFLAGS) $(DEFINES) $(REFS) > .depend

clean:	
	rm -f $(OBJS) *~ $(LIBNAME) core
	
%.o: %.cpp
	$(CC) $(CDLFLAGS) $(DEFINES) -c $< -o $@

ifeq (.depend,$(wildcard .depend))
include .depend
endif
