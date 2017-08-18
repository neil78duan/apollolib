
topdir=(shell pwd)
VPATH = .

include ./Rules.make

SUBDIRS =  cli_common srv_common attribute logic_parser


all: debug

debug:
	for n in $(SUBDIRS); do $(MAKE) -C $$n DEBUG="y" PROFILE="y" || exit 1; done
	cd tool ; make debug 

release:
	for n in $(SUBDIRS); do $(MAKE) -C $$n DEBUG="n" PROFILE="n" || exit 1; done
	cd tool ; make release

tool:
	cd tool ;make 

clean:
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean DEBUG="n" PROFILE="n"; done
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean DEBUG="y" PROFILE="y" || exit 1; done

config:
	chmod u+x ./config.sh ; ./config.sh
