include ../MakeConf/inc.mk

#test_subdirs=$(shell ls | sed -e "s/Makefile//")

test_subdirs=test_phong test_shadow

all:subdirs
	
subdirs:
ifeq ($(MAKE_TESTS), true)
	for item in $(test_subdirs); do cd $$item && $(MAKE); cd ..; done
else	
	echo "NO MAKE TEST FLAG IN MAKE_CONFIG"	
endif
clean:
	for item in $(test_subdirs); do cd $$item && $(MAKE_CLEAN);  cd ..; done

.PHONY:subdirs





	
