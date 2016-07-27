# c test  Makefile 
# 2005-11-6 
# neil duan 

#set path of src and obj


SRCDIR = ./protocolBuilder
OBJDIR = .obj
include ../../Rules.make


PROJECT_NAME := protoBuilder

SRCDIR=./protocolBuilder


ifeq ($(DEBUG),y)
	PROJS = $(WORKDIR)/protoBuilder_d
else
	PROJS = $(WORKDIR)/protoBuilder_d
endif

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
	