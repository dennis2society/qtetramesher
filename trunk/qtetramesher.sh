#!/bin/bash
# You probably want to edit the following line to match the path to you SOFA instance. Don't forget to append the 'lib' folder.
SOFA_LIBS_PATH=/opt/sofa/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SOFA_LIBS_PATH:CGALTetrahedralize/lib:SofaTetraStuffing/lib:TetraMeshTools/lib
./QTetraMesher

