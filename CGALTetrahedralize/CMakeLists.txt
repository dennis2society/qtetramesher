cmake_minimum_required(VERSION 3.16)
project(CGALTetrahedralize VERSION 1.0)

find_package(CGAL REQUIRED)

include_directories(
  ../TetraMeshTools/include
  ../TetraMeshTools/include/TetraMeshTools
  PRIVATE
  include
)

add_library(
  CGALTetrahedralize
  SHARED
  src/CGALTetrahedralize.cpp
)

target_link_libraries(
  CGALTetrahedralize
  CGAL
)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -fopenmp)

if(LINUX)
  add_custom_command(
    TARGET CGALTetrahedralize
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      "$<TARGET_FILE:CGALTetrahedralize>"
      "${CMAKE_BINARY_DIR}"
  )
endif()
