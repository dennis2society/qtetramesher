cmake_minimum_required(VERSION 3.16)
project(TetraMeshTools VERSION 1.0)

# This can/should be done by providing the CMAKE_PREFIX_PATH="my_sofa_path"
list(APPEND CMAKE_PREFIX_PATH "/opt/sofa_git/build/lib/cmake")
list(APPEND CMAKE_PREFIX_PATH "/opt/sofa_git/build/install/lib/cmake")

find_package(Sofa REQUIRED)
find_library(SofaCore
             NAMES Sofa.Core
             HINTS ${SOFA_ROOT}/lib
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
  ${SOFA_ROOT}/include/Sofa.Config
  ${SOFA_ROOT}/include/Sofa.Helper
  ${SOFA_ROOT}/include/Sofa.Type
  ${SOFA_ROOT}/include/Sofa.Core
  ${SOFA_ROOT}/include/Sofa.Topology
  ${SOFA_ROOT}/include/Sofa.Geometry
  ${SOFA_ROOT}/include/Sofa.DefaultType
  ${SOFA_ROOT}/include/Sofa.LinearAlgebra
  ${SOFA_ROOT}/include/Sofa.Component.Compat
  ${SOFA_ROOT}/include/Sofa.Component.Engine.Generate
)

add_library(
  SofaTetraStuffing
  SHARED
  src/SofaTetraStuffing.cpp
)

link_directories(
  ${SOFA_ROOT}/lib
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
