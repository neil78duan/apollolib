#make file 

SRCDIR=.
OBJDIR = ./.objs

include ../Rules.make


##############set flag

CFLAGS += -I$(TOPDIR)/include -I$(TOPDIR)/logic_parser

ifeq ($(DEBUG),y)
    PROJS = $(LIBDIR)/librole_attribute_d.a
else 
    PROJS = $(LIBDIR)/librole_attribute.a
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
	
