# Makefile for TetraMeshTools library
# Author: Dennis Luebke, 2012-08-31

# source files
dirSrc:= src
dirTrimesh2:= trimesh2
# local headers
incDir:= include
# local headers
ttmDir:= include/TetraMeshTools

outputDir:= lib
outputName:= libTetraMeshTools.so

CFLAGS:=        -O2 -pipe 
CFLAGS+=        -fopenmp
#CFLAGS+=       -g
#CFLAGS+=       -O0
CFLAGS+=        -I$(incDir)
CFLAGS+=		-I$(ttmDir)
CFLAGS+=		-I$(dirTrimesh2)/include
CFLAGS+=        -fpic
CFLAGS+=        -shared 

CXXFLAGS:=      $(CFLAGS)

# make targets
all:	
	$(CXX) $(CXXFLAGS) $(dirSrc)/* $(dirTrimesh2)/src/* -o $(outputDir)/$(outputName)
	
clean:
	rm -rf lib/*
