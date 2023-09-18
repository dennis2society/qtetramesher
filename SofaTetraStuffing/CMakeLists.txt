cmake_minimum_required(VERSION 3.16)
project(SofaTetraStuffing VERSION 1.0)

# Set this to where you have installed your Sofa build:
# (This is only needed if you want to build this lib from here)
set(SOFA_AUX_PATH "/opt/sofa_git/build/install/" CACHE PATH "")

if (NOT DEFINED SOFA_INSTALL_DIR)
  message("Setting  SOFA_INSTALL_DIR...")
  set(SOFA_INSTALL_DIR ${SOFA_AUX_PATH})
  message("SOFA_INSTALL_DIR set to: " "${SOFA_INSTALL_DIR}")
else()
  message("SOFA_INSTALL_DIR already set to: " "${SOFA_INSTALL_DIR}")
endif()

list(APPEND CMAKE_PREFIX_PATH ${SOFA_INSTALL_DIR}/lib/cmake)

find_package(Sofa.Config REQUIRED)

find_library(SofaCore
             NAMES Sofa.Core
             HINTS ${SOFA_INSTALL_DIR}/lib
)

if(NOT SofaCore)
  message(FATAL_ERROR "SofaCore library not found")
endif()

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

if(LINUX OR UNIX)
  add_custom_command(
    TARGET SofaTetraStuffing
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      "$<TARGET_FILE:SofaTetraStuffing>"
      "${CMAKE_BINARY_DIR}"
  )
endif()
