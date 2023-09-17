#!/bin/bash
# You probably want to edit the following line to match the path to you SOFA instance. Don't forget to append the 'lib' folder.
SOFA_LIBS_PATH=/opt/sofa_git/build/install/lib
cd bin_linux_x64
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib:$SOFA_LIBS_PATH::.
./QTetraMesher
