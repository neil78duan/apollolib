#make file 

SRCDIR=.
OBJDIR = ./.objs

include ../Rules.make

#create objdir
objdir := $(shell [ -d obj ] || mkdir obj) 

##############set flag


ifeq ($(DEBUG),y)
    PROJS = $(LIBDIR)/libnetMessage_d.a
else 
    PROJS = $(LIBDIR)/libnetMessage.a
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
	
