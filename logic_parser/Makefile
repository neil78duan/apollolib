#make file 

SRCDIR=.
OBJDIR = ./.objs

include ../Rules.make


##############set flag

#CFLAGS += -I$(TOPDIR)/netMessage -I$(TOPDIR)/include

ifeq ($(DEBUG),y)
    PROJS = $(LIBDIR)/liblogic_parser_d.a
else 
    PROJS = $(LIBDIR)/liblogic_parser.a
endif

CFLAGS += -I$(TOPDIR)/include
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
	
