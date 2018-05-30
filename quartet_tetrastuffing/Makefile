# Makefile for QuartetTetraStuffing library
# Author: Dennis Luebke, 2018-05-29

# source files
dirSrc:= src
# local headers
incDir:= include

# TetraMeshTools path
ttmDir:= ../TetraMeshTools
ttmLibDir:= $(ttmDir)/lib

outputDir:= lib
outputName:= libQuartetTetraStuffing.so

CFLAGS:=        -O2 -pipe 
CFLAGS+=        -fopenmp
#CFLAGS+=       -g
#CFLAGS+=       -O0
CFLAGS+=        -I$(incDir)
CFLAGS+=		-I$(ttmDir)/include -I$(ttmDir)/include/TetraMeshTools
CFLAGS+=		-I$(sofaDir)/include # -I$(sofaDir)/modules -I$(sofaDir)/framework
CFLAGS+=		-I$ttmInclude
CFLAGS+=        -fpic
CFLAGS+=        -shared 

CXXFLAGS:=      $(CFLAGS)

# TetraMeshTools lib path and files
ttmLibs:= -L$(ttmLibDir) -lTetraMeshTools

# make targets
all:	
	$(CXX) $(CXXFLAGS) $(dirSrc)/*.cpp -o $(outputDir)/$(outputName) $(ttmLibs)
	
clean:
	rm -rf $(outputDir)/$(outputName)
