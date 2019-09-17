#make file 

SRCDIR=.
OBJDIR = ./.objs

include ../Rules.make


##############set flag

CFLAGS += -I$(TOPDIR)/include -I$(TOPDIR)/logic_parser 

ifeq ($(DEBUG),y)
    PROJS = $(LIBDIR)/libgame_parser_d.a
else 
    PROJS = $(LIBDIR)/libgame_parser.a
endif

ifeq ($(BUILD_DLL),y)
	LFLAGS += -l$(COMMON_DLL) -l$(NF_PARSER)
	CFLAGS += -fPIC
endif

PLUGOBJ := dbl_mgr.o dbl_plugin.o
PATH_PLUGOBJ := $(OBJDIR)/dbl_mgr.o $(OBJDIR)/dbl_plugin.o
PLUGIN_NAME := $(LIBDIR)/libdbl_plugin.$(DLL_EXT_NAME)
##########make 
##############################################################


all: $(PROJS)
	
$(PROJS): $(OBJS)
	$(AR) $(PROJS)  $(PathOBJS)  

dll: $(PLUGIN_NAME)

$(PLUGIN_NAME):$(PLUGOBJ)
	$(CPP_DLL) -fPIC -o $(PLUGIN_NAME)  $(PATH_PLUGOBJ) $(LFLAGS) -liconv

.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) -o $(OBJDIR)/$*.o $<
.cpp.o:
	$(CPP) $(INCLUDE) $(CFLAGS) $(CPPFLAGS) -o $(OBJDIR)/$*.o $<
	
clean-dll:
	rm -rf $(PATH_PLUGOBJ) PLUGIN_NAME
clean:
	rm -f $(PROJS) $(PathOBJS)  $(PathOBJS:.o=.d)
	
