include ../../default.mak

OBJS = EntropyBase.o EntropyPackets.o EntropyRoom.o
REFS = EntropyBase.cpp EntropyPackets.cpp EntropyRoom.cpp
LIBNAME = ../../netgames/entropy.d3m

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
