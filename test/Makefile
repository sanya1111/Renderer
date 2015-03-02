include ../MakeConf/inc.mk
test_subdirs=$(shell ls | sed -e "s/Makefile//")

all:subdirs
	
subdirs:
	for item in $(test_subdirs); do cd $$item && $(MAKE); cd ..; done	
clean:
	for item in $(test_subdirs); do cd $$item && $(MAKE_CLEAN);  cd ..; done

.PHONY:subdirs





	
