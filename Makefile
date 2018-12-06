
topdir=(shell pwd)
VPATH = .

include ./Rules.make

SUBDIRS = cli_common srv_common db_common attribute game_parser


all: debug

debug:
	for n in $(SUBDIRS); do $(MAKE) -C $$n DEBUG="y" PROFILE="y" || exit 1; done
	cd tool ; make debug 

release:
	for n in $(SUBDIRS); do $(MAKE) -C $$n DEBUG="n" PROFILE="n" || exit 1; done
	cd tool ; make release

dll: dll_release

dll_debug:
	cd game_parser ; make dll DEBUG="y" PROFILE="y" BUILD_DLL="y"

dll_release:
	cd game_parser ; make dll DEBUG="n" PROFILE="n" BUILD_DLL="y"

tool:
	cd tool ;make 

clean_dll:
	cd game_parser ;make clean_dll

clean:
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean DEBUG="n" PROFILE="n"; done
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean DEBUG="y" PROFILE="y" || exit 1; done
	cd tool; make clean 

config:
	chmod u+x ./config.sh ; ./config.sh
