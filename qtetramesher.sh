#!/bin/bash
# Update this line to match your current Sofa installation!
SOFA_LIBS_PATH=/opt/sofa_git/build/install/lib
cd build
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib:$SOFA_LIBS_PATH:.
./QTetraMesher
