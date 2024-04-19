include ../../default.mak

OBJS = coop.o
REFS = coop.cpp
LIBNAME = ../../netgames/co-op.d3m

CDLFLAGS = $(CFLAGS) -fPIC
LDLFLAGS = -shared -ldl

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
