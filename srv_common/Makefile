#make file 

SRCDIR=.
OBJDIR = ./.objs

include ../Rules.make


##############set flag

CFLAGS += -I$(TOPDIR)/include -I$(TOPDIR)/include/mysql

ifeq ($(DEBUG),y)
    PROJS = $(LIBDIR)/libatls_srv_common_d.a
else 
    PROJS = $(LIBDIR)/libatls_srv_common.a
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
	
