include ../MakeConf/inc.mk
test_subdirs=$(shell ls | sed -e "s/Makefile//")

all:subdirs
	
subdirs:
	for item in $(test_subdirs); do cd $$item && $(MAKE); done	
clean:
	for item in $(test_subdirs); do cd $$item && $(MAKE_CLEAN); done

.PHONY:subdirs





	
