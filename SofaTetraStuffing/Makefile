# Makefile for SofaTetraStuffing library
# Author: Dennis Luebke, 2012-09-02

# source files
dirSrc:= src
# local headers
incDir:= include

# TetraMeshTools path
ttmDir:= ../TetraMeshTools
ttmLibDir:= $(ttmDir)/lib

# SOFA root dir (change to where your own SOFA instance is living)
sofaDir:= ../../sofa
sofaLibDir:=$(sofaDir)/lib

outputDir:= lib
outputName:= libSofaTetraStuffing.so

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

# SOFA lib path and files
sofaLibs:= -L$(sofaLibDir) -lSofaMisc -lSofaComponentMisc -lSofaCore

# make targets
all:	
	$(CXX) $(CXXFLAGS) $(dirSrc)/*.cpp -o $(outputDir)/$(outputName) $(sofaLibs) $(ttmLibs)
	
clean:
	rm -rf $(outputDir)/$(outputName)
