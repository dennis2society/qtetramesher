# Makefile for CGALTetrahedralize library
# Author: Dennis Luebke, 2012-12-06

# source files
dirSrc:= src
# local headers
incDir:= include

# TetraMeshTools path
ttmDir:= ../TetraMeshTools
ttmLibDir:= $(ttmDir)/lib

outputDir:= lib
outputName:= libCGALTetrahedralize.so

CFLAGS:=        -O2 -pipe 
CFLAGS+=        -fopenmp
#CFLAGS+=       -g
#CFLAGS+=       -O0
CFLAGS+=        -I$(incDir)
CFLAGS+=		-I$(ttmDir)/include -I$(ttmDir)/include/TetraMeshTools
CFLAGS+=		-I/usr/include
CFLAGS+=		-I$ttmInclude
CFLAGS+=        -fpic
CFLAGS+=        -shared 

CXXFLAGS:=      $(CFLAGS)

# TetraMeshTools lib path and files
ttmLibs:= -L$(ttmLibDir) -lTetraMeshTools

# SOFA lib path and files
libs:= -L/usr/lib/CGAL -L/usr/lib -L/usr/lib/i386-linux-gnu
libs+= -lCGAL -lboost_thread -lgmp -lmpfr -lCGAL_Core

# make targets
all:	
	$(CXX) $(CXXFLAGS) $(dirSrc)/* -o $(outputDir)/$(outputName) $(libs) $(ttmLibs)
	
clean:
	rm -rf $(outputDir)/$(outputName)
