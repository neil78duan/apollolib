#make file 

SRCDIR = .
OBJDIR = .obj
include ../../Rules.make


##############set flag

CFLAGS += -I$(TOPDIR)/include -I$(TOPDIR)/cli_common
LFLAGS += -L$(LIBDIR)

ifeq ($(DEBUG),y)
LFLAGS +=  -lapollo_cli_common_d
else
LFLAGS +=  -lapollo_cli_common
endif

PROJS = $(WORKDIR)/robort

LFLAGS += -l$(CLIENT_LIB)


LFLAGS += -l$(SRV_LIB)
MYLIBS := $(ND_SRV_LIB)


##########make 

all: $(PROJS) 
			
$(PROJS): $(OBJS) 
	$(CPP) -o  $(PROJS)   $(PathOBJS)  $(LFLAGS)

                
.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) -o $(OBJDIR)/$*.o $<
.cpp.o:
	$(CPP) $(INCLUDE) $(CFLAGS) -o $(OBJDIR)/$*.o $<
	

clean:
	rm -f $(PROJS) $(PathOBJS)  $(PathOBJS:.o=.d)

