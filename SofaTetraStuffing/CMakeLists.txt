cmake_minimum_required(VERSION 3.16)
project(TetraMeshTools VERSION 1.0)

# Set this to where you have installed your Sofa build:
set(SOFA_INSTALL_DIR "/opt/sofa_git/build/install/")

# This can/should be done by providing the CMAKE_PREFIX_PATH="my_sofa_path"
list(APPEND CMAKE_PREFIX_PATH ${SOFA_INSTALL_DIR}/lib/cmake)


find_package(Sofa.Config REQUIRED)

find_library(SofaCore
             NAMES Sofa.Core
             HINTS ${SOFA_INSTALL_DIR}/lib
)

if(NOT SofaCore)
  message(FATAL_ERROR "SofaCore library not found")
endif()

message(${SofaCore})

include_directories(
  ../TetraMeshTools/include
  ../TetraMeshTools/include/TetraMeshTools
  PRIVATE
  include
  ${SOFA_INSTALL_DIR}/include/Sofa.Config
  ${SOFA_INSTALL_DIR}/include/Sofa.Helper
  ${SOFA_INSTALL_DIR}/include/Sofa.Type
  ${SOFA_INSTALL_DIR}/include/Sofa.Core
  ${SOFA_INSTALL_DIR}/include/Sofa.Topology
  ${SOFA_INSTALL_DIR}/include/Sofa.Geometry
  ${SOFA_INSTALL_DIR}/include/Sofa.DefaultType
  ${SOFA_INSTALL_DIR}/include/Sofa.LinearAlgebra
  ${SOFA_INSTALL_DIR}/include/Sofa.Component.Compat
  ${SOFA_INSTALL_DIR}/include/Sofa.Component.Engine.Generate
)

add_library(
  SofaTetraStuffing
  SHARED
  src/SofaTetraStuffing.cpp
)

link_directories(
  ${SOFA_INSTALL_DIR}/lib
)

target_link_libraries(
  SofaTetraStuffing
  ${SofaCore}
)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -fopenmp)

if(LINUX)
  add_custom_command(
    TARGET SofaTetraStuffing
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
      -E copy $<TARGET_FILE:SofaTetraStuffing>
      ${CMAKE_SOURCE_DIR}/../bin_linux_x64/
  )
endif()
