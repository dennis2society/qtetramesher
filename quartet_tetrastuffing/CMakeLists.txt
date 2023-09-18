cmake_minimum_required(VERSION 3.16)
project(QuartetTetraStuffing VERSION 1.0)

include_directories(
  ../TetraMeshTools/include
  ../TetraMeshTools/include/TetraMeshTools
  PRIVATE
  include
  src
)

add_library(
  QuartetTetraStuffing
  SHARED
  src/feature.cpp
  src/make_signed_distance.cpp
  src/match_features.cpp
  src/predicates.cpp
  src/sdf.cpp
  src/tet_quality.cpp
  src/geometry_queries.cpp
  src/make_tet_mesh.cpp
  src/optimize_tet_mesh.cpp
  src/QuartetTetraStuffing.cpp
  src/tet_mesh.cpp
  src/trimesh.cpp
)

target_link_libraries(
  QuartetTetraStuffing
)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -fopenmp)

if(LINUX)
  add_custom_command(
    TARGET QuartetTetraStuffing
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      "$<TARGET_FILE:QuartetTetraStuffing>"
      "${CMAKE_BINARY_DIR}"
  )
endif()
