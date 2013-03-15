#!/bin/bash
SOFA_LIBS_PATH=/opt/sofa/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SOFA_LIBS_PATH:CGALTetrahedralize/lib:SofaTetraStuffing/lib:TetraMeshTools/lib
./QTetraMesher

