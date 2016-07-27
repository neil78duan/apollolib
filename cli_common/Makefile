#make file 

SRCDIR=.
OBJDIR = ./.objs

include ../Rules.make


##############set flag

CFLAGS += -I$(TOPDIR)/include  -DWITHOUT_LOGIC_PARSER

ifeq ($(DEBUG),y)
    PROJS = $(LIBDIR)/libapollo_cli_common_d.a
else 
    PROJS = $(LIBDIR)/libapollo_cli_common.a
endif

##########make 
##############################################################


all: $(PROJS) 
	
$(PROJS): $(OBJS)
	$(AR) $(PROJS)  $(PathOBJS) $(LibFLAGS) 

.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) -o $(OBJDIR)/$*.o $<
.cpp.o:
	$(CPP) $(INCLUDE) $(CFLAGS) -o $(OBJDIR)/$*.o $<
	

clean:
	rm -f $(PROJS) $(PathOBJS)  $(PathOBJS:.o=.d)
	
