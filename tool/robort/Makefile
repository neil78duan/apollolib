#make file 

SRCDIR = .
OBJDIR = .obj
include ../../Rules.make


##############set flag

PROJECT_NAME := apollo_robort


CFLAGS += -I$(TOPDIR)/include  -I$(TOPDIR)/src/cli_common


ifeq ($(DEBUG),y)
LFLAGS +=  -lapollo_cli_common_d
PROJS = $(WORKDIR)/robort_d
else
LFLAGS +=  -lapollo_cli_common
PROJS = $(WORKDIR)/robort
endif

LFLAGS += -l$(CLIENT_LIB)



##########make 
##############################################################


CFLAGS += -I$(TOPDIR)/include  -I$(PROJ_SRC)/common \
	-I$(PROJ_SRC)/attribute -I$(PROJ_SRC)/logic_compile

LFLAGS += -l$(SRV_LIB)
MYLIBS := $(ND_SRV_LIB)

all: $(PROJS) 
			
$(PROJS): $(OBJS) 
	$(CPP) -o  $(PROJS)   $(PathOBJS)  $(LFLAGS)

                
.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) -o $(OBJDIR)/$*.o $<
.cpp.o:
	$(CPP) $(INCLUDE) $(CFLAGS) -o $(OBJDIR)/$*.o $<
	

clean:
	rm -f $(PROJS) $(PathOBJS)  $(PathOBJS:.o=.d)

