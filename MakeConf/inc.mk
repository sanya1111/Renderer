#VARS
CC=g++-4.9
CC_FLAGS=--std=c++0x -Wall -Wextra -Wno-reorder -O3 -D__cplusplus=201103L
LIB_DIR=lib
SRC_DIR=src
TEST_DIR=test
INCLUDE_DIR=inc
OBJ_DIR=obj
BIN_DIR=bin
MAKE_CLEAN=make clean
INCLUDE_EXTERNAL_DIRS=/usr/include/libkms /usr/include/libdrm
EXTERNAL_LIBS=kms drm
DEBUB_LOG=-DDEBUG_LOG
CC_PROJECT_PARAMS=$(shell for i in $(INCLUDE_EXTERNAL_DIRS); do echo "-I$$i "; done) $(shell for i in $(EXTERNAL_LIBS); do echo "-l$$i "; done) $(DEBUB_LOG)
MAKE_TESTS=true
#FUNCTIONS
get_objects=$(patsubst  $1/%.cpp,$1/%.o,$(wildcard $1/*.cpp))
