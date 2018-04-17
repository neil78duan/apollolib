#make file 

SRCDIR=.
OBJDIR = ./.objs

include ../Rules.make


##############set flag

CFLAGS += -I$(TOPDIR)/include -I$(TOPDIR)/logic_parser -I$(TOPDIR)/include/mysql

ifeq ($(DEBUG),y)
    PROJS = $(LIBDIR)/libhttp_apo_d.a
else 
    PROJS = $(LIBDIR)/libhttp_apo.a
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
	
