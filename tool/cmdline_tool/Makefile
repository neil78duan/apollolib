# c test  Makefile 
# 2005-11-6 
# neil duan 

include ../../Rules.make

#PROJ1  = $(WORKDIR)/client_test
PROJ2  = $(WORKDIR)/cmdline_tool

#OBJS   :=   commonTest.o testConnector.o

TOOLOBJS := $(OBJS) client_cmdline.o test_msgsend.o atlantis_test.o rsa_gen.o commonTest.o

CFLAGS += -I$(TOPDIR)/include  -I$(LIBAPOLLO)/cli_common -DWITHOUT_LOGIC_PARSER

MYLIBS := $(NDHOME)/lib/lib$(CLIENT_LIB).a


ifeq ($(DEBUG),y)
	LFLAGS +=  -lapollo_cli_common_d
else
	LFLAGS +=  -lapollo_cli_common
endif

LFLAGS += -L$(LIBDIR) -l$(CLIENT_LIB)


VPATH := .

all:  $(PROJ2)
			
# $(PROJ1): $(OBJS) main.o  $(MYLIBS)
#	$(CPP) -o  $(PROJ1)  main.o  $(OBJS)  $(LFLAGS)

$(PROJ2): $(TOOLOBJS) $(MYLIBS)
	$(CPP) -o  $(PROJ2) $(TOOLOBJS) $(LFLAGS)


.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) -o $*.o $<
.cpp.o:
	$(CC) $(INCLUDE) $(CFLAGS) -o $*.o $<
	
clean:
	rm -f *.o  $(PROJS) $(OBJS:.o=.d)
	
