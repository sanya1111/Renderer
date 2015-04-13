#VARS
CXX?=g++-4.9
CXX_FLAGS=--std=c++11 -Wall -Wextra -Wno-reorder -O3
LIB_DIR=lib
SRC_DIR=src
TEST_DIR=test
INCLUDE_DIR=inc
OBJ_DIR=obj
BIN_DIR=bin
MAKE_CLEAN=make clean
INCLUDE_EXTERNAL_DIRS=/usr/include/libkms /usr/include/libdrm /usr/local/include/assimp
EXTERNAL_LIBS=kms drm assimp
DEBUB_LOG=-DDEBUG_LOG
CXX_PROJECT_PARAMS=$(shell for i in $(INCLUDE_EXTERNAL_DIRS); do echo "-I$$i "; done) $(DEBUB_LOG)
CXX_PROJECT_LIBS=$(shell for i in $(EXTERNAL_LIBS); do echo "-l$$i "; done)
MAKE_TESTS=true
#FUNCTIONS
get_objects=$(patsubst  $1/%.cpp,$1/%.o,$(wildcard $1/*.cpp))
