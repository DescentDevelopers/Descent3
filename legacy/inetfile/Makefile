include ../../default.mak

OBJS = CFtp.o Chttpget.o inetgetfile.o 
REFS = CFtp.cpp Chttpget.cpp inetgetfile.cpp 
#OBJS = lnxinetfile.o
#REFS = lnxinetfile.cpp
LIBNAME = inetfile.a

all:	depall $(OBJS)
	ar r $(LIBNAME) $(OBJS)
	ranlib $(LIBNAME)

depall:
	$(CC) -E -M $(CFLAGS) $(DEFINES) $(REFS) > .depend

clean:	
	rm -f $(OBJS) *~ $(LIBNAME) core
	
%.o: %.cpp
	$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@

ifeq (.depend,$(wildcard .depend))
include .depend
endif