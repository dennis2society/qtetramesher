cmake_minimum_required(VERSION 3.16)
project(QTetraMesher VERSION 0.8.3)

# Set this to where you have installed your Sofa build:
set(SOFA_INSTALL_DIR "/opt/sofa_git/build/install/" CACHE PATH "")

list(APPEND CMAKE_PREFIX_PATH ${SOFA_INSTALL_DIR}/lib/cmake)


find_package(Sofa.Config REQUIRED)

find_library(SofaCore
             NAMES Sofa.Core
             HINTS ${SOFA_INSTALL_DIR}/lib
)

if(NOT SofaCore)
  message(FATAL_ERROR "SofaCore library not found")
endif()

add_subdirectory(TetraMeshTools)
add_subdirectory(SofaTetraStuffing)
add_subdirectory(CGALTetrahedralize)
add_subdirectory(quartet_tetrastuffing)
