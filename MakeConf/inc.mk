#VARS
CC=g++
CC_FLAGS=--std=c++0x -Wall -Wextra
LIB_DIR=lib
SRC_DIR=src
TEST_DIR=test
INCLUDE_DIR=include
OBJ_DIR=obj
BIN_DIR=bin
MAKE_CLEAN=make clean

#FUNCTIONS
get_objects=$(patsubst  $1/%.cpp,$1/%.o,$(wildcard $1/*.cpp))
