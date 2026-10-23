cmake_minimum_required(VERSION 3.16)
project(GenericIsosurfaceStuffing VERSION 1.0)

include_directories(
  PRIVATE
  include
  src
)

add_library(
  GenericIsosurfaceStuffing
  SHARED
  include/GenericIsosurfaceStuffing.h
  src/GenericIsosurfaceStuffing.cpp
  src/BCCLattice.h
  src/BCCLattice.cpp
  src/MeshOptimizer.h
  src/MeshOptimizer.cpp
  src/SignedDistanceField.h
  src/SignedDistanceField.cpp
  src/TetStuffer.h
  src/TetStuffer.cpp
)

target_link_libraries(${PROJECT_NAME} TetraMeshTools::Headers)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
if(NOT MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -O3 -Wall -Wextra -Wpedantic)
endif()

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
endif()

add_custom_command(
  TARGET GenericIsosurfaceStuffing
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:GenericIsosurfaceStuffing>"
    "${CMAKE_BINARY_DIR}"
)
