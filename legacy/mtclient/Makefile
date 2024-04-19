include ../../default.mak

OBJS = chat_api.o mt_net.o mtclient.o mtgametrack.o mtpilottracker.o
REFS = chat_api.cpp mt_net.cpp mtclient.cpp mtgametrack.cpp mtpilottracker.cpp
LIBNAME = mtclient.so
CDLFLAGS = $(CFLAGS) -fPIC
LDLFLAGS = -shared
all:	depall $(OBJS)
	gcc $(LDLFLAGS) -o $(LIBNAME) $(OBJS)
	mv mtclient.so "../../online/Parallax Online.d3c"

depall:
	$(CC) -E -M $(CFLAGS) $(DEFINES) $(REFS) > .depend

clean:	
	rm -f $(OBJS) *~ "../../online/Parallax Online.d3c" core
	
%.o: %.cpp
	$(CC) $(CDLFLAGS) $(DEFINES) -c $< -o $@

ifeq (.depend,$(wildcard .depend))
include .depend
endif
