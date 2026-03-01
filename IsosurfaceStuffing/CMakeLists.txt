cmake_minimum_required(VERSION 3.16)
project(IsosurfaceStuffing VERSION 1.0)

include_directories(
  ../TetraMeshTools/include
  ../TetraMeshTools/include/TetraMeshTools
  PRIVATE
  include
  src
)

add_library(
  IsosurfaceStuffing
  SHARED
  include/IsosurfaceStuffing.h
  src/IsosurfaceStuffing.cpp
  src/BCCLattice.h
  src/BCCLattice.cpp
  src/SignedDistanceField.h
  src/SignedDistanceField.cpp
  src/CutPointSnapper.h
  src/CutPointSnapper.cpp
  src/TetStuffer.h
  src/TetStuffer.cpp
  src/GradedBCCLattice.h
  src/GradedBCCLattice.cpp
  src/Warp.h
  src/Warp.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
add_compile_options(-O3 -Wall -Wextra -Wpedantic)

add_custom_command(
  TARGET IsosurfaceStuffing
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:IsosurfaceStuffing>"
    "${CMAKE_BINARY_DIR}"
)
