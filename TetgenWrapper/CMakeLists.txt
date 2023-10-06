cmake_minimum_required(VERSION 3.16)
project(TetgenWrapper VERSION 0.5)

add_compile_definitions(TETLIBRARY)

include_directories(
  ../TetraMeshTools/include
  ../TetraMeshTools/include/TetraMeshTools
  src
  PRIVATE
  include
)

add_library(
  TetgenWrapper
  SHARED
  include/TetgenWrapper.h
  src/TetgenWrapper.cpp
  src/tetgen.h
  src/tetgen.cxx
  src/predicates.cxx
)

target_link_libraries(
  TetgenWrapper
)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -fopenmp)

add_custom_command(
  TARGET TetgenWrapper
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:TetgenWrapper>"
    "${CMAKE_BINARY_DIR}"
)
