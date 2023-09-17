cmake_minimum_required(VERSION 3.16)
project(TetraMeshTools VERSION 1.0)

find_package(assimp REQUIRED NO_MODULE)

include_directories(
  TetraMeshTools
  PRIVATE
  include
  include/TetraMeshTools
  trimesh2/include
)

add_library(
  TetraMeshTools
  SHARED
  src/BaseMeshLoader.cpp
  src/GMSHMeshLoader.cpp
  src/GMSHMeshWriter.cpp
  src/Octree.cpp
  src/OctreeNode.cpp
  src/SATriangleBoxIntersection.cpp
  src/TetgenWriter.cpp
  src/TetrahedronTopology.cpp
  src/TriangleTopology.cpp
  src/TriMeshLoader.cpp
  src/TriMeshWriter.cpp
  trimesh2/src/conn_comps.cc
  trimesh2/src/filter.cc
  trimesh2/src/overlap.cc
  trimesh2/src/subdiv.cc
  trimesh2/src/TriMesh_grid.cc
  trimesh2/src/TriMesh_stats.cc
  trimesh2/src/diffuse.cc
  trimesh2/src/ICP.cc
  trimesh2/src/remove.cc
  trimesh2/src/TriMesh_bounding.cc
  trimesh2/src/TriMesh_io.cc
  trimesh2/src/TriMesh_tstrips.cc
  trimesh2/src/edgeflip.cc
  trimesh2/src/KDtree.cc
  trimesh2/src/reorder_verts.cc
  trimesh2/src/TriMesh_connectivity.cc
  trimesh2/src/TriMesh_normals.cc
  trimesh2/src/faceflip.cc
  trimesh2/src/lmsmooth.cc
  trimesh2/src/shared.cc
  trimesh2/src/TriMesh_curvature.cc
  trimesh2/src/TriMesh_pointareas.cc
)

target_link_libraries(
  TetraMeshTools
  assimp
)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -fopenmp)

if(LINUX)
  add_custom_command(
    TARGET TetraMeshTools
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
      -E copy $<TARGET_FILE:TetraMeshTools>
      ${CMAKE_SOURCE_DIR}/../bin_linux_x64/
  )
endif()