cmake_minimum_required(VERSION 3.16)
project(TetraMeshTools VERSION 1.0)

find_package(assimp REQUIRED)

include_directories(
  TetraMeshTools
  PRIVATE
  include
  include/TetraMeshTools
  ${ASSIMP_INCLUDE_DIR}
)

add_library(
  TetraMeshTools
  SHARED
  include/TetraMeshTools/BaseMeshLoader.h
  include/TetraMeshTools/SATriangleBoxIntersection.h
  include/TetraMeshTools/SurfaceChecker.h
  include/TetraMeshTools/TetrahedronTopology.h
  include/TetraMeshTools/TetraMeshQuality.h
  include/TetraMeshTools/TriMeshLoader.h
  include/TetraMeshTools/Vec3f.h
  include/TetraMeshTools/Vec3d.h
  include/TetraMeshTools/GeometryTypes.h
  include/TetraMeshTools/MeshRepair.h
  include/TetraMeshTools/MeshTransform.h
  include/TetraMeshTools/Octree.h
  include/TetraMeshTools/TetraToolsExports.h
  include/TetraMeshTools/TriMeshWriter.h
  include/TetraMeshTools/Vec4f.h
  include/TetraMeshTools/OctreeNode.h
  include/TetraMeshTools/TriangleTopology.h
  include/TetraMeshTools/Vec2f.h
  src/BaseMeshLoader.cpp
  src/TopologyUtils.h
  src/MeshRepair.cpp
  src/MeshTransform.cpp
  src/Octree.cpp
  src/OctreeNode.cpp
  src/SATriangleBoxIntersection.cpp
  src/SurfaceChecker.cpp
  src/TetrahedronTopology.cpp
  src/TetraMeshQuality.cpp
  src/TriangleTopology.cpp
  src/TriMeshLoader.cpp
  src/TriMeshWriter.cpp
)

# Header-only target that only provides the include path. Libraries that use
# the TetraMeshTools types (Vec3f, Triangle, ...) without calling into the
# library link this instead of TetraMeshTools.
add_library(TetraMeshToolsHeaders INTERFACE)
add_library(TetraMeshTools::Headers ALIAS TetraMeshToolsHeaders)
target_include_directories(TetraMeshToolsHeaders
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
  TetraMeshTools
  TetraMeshTools::Headers
  assimp::assimp
)

if(MSVC)
  add_compile_options(/openmp)
else()
  add_compile_options(-O3 -Wall -Wextra -Wpedantic -fopenmp)
endif()

add_custom_command(
  TARGET TetraMeshTools
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:TetraMeshTools>"
    "${CMAKE_BINARY_DIR}"
)
