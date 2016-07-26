# c test  Makefile 
# 2005-11-6 
# neil duan 

#set path of src and obj


#debug flag
DEBUG =y
#profile , hot spot find
PROFILE=n

SRCDIR=./protocolBuilder
objdir := $(shell [ -d .objs ] || mkdir .objs)
OBJDIR = ./.objs

include ./nd_lib_include.make


PROJS  = ./bin/protoBuilder



CFLAGS +=  -I$(NDHOME)/include

LFLAGS +=   -L$(NDHOME)/lib -l$(CLIENT_LIB)


all: $(PROJS) 

$(PROJS): $(OBJS)  $(MYLIBS)
	$(CPP) -o  $(PROJS)   $(PathOBJS)  $(LFLAGS)

.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) -o $(OBJDIR)/$*.o $<
.cpp.o:
	$(CPP) $(INCLUDE) $(CFLAGS) -o $(OBJDIR)/$*.o $<
	
.cc.o:
	$(CPP) $(INCLUDE) $(CFLAGS) -o $(OBJDIR)/$*.o $<

clean:
	rm -f $(PROJS) $(PathOBJS)  $(PathOBJS:.o=.d)

init:
	cp ../../server/protocol/src/*.cpp ./roborts
	rm ./roborts/MessageSender.cpp
	