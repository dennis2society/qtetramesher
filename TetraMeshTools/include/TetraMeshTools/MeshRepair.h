/*
 * MeshRepair.h
 *
 * Automatic repair of triangle surface meshes, so that loaded meshes become
 * usable as input for the volumetric mesh generators (Tetgen, GMSH, ...).
 *
 * Repaired: split/near-coincident vertices, degenerate and duplicate
 * triangles, inconsistent and inverted orientation, simple holes and unused
 * vertices. Not repaired: self-intersections and non-manifold edges (these
 * are only counted, use CheckSurfaceMesh() for a detailed analysis).
 */

#ifndef MESH_REPAIR_H
#define MESH_REPAIR_H

#include <TetraMeshTools/GeometryTypes.h>
#include <TetraMeshTools/TetraToolsExports.h>

#include <cstddef>
#include <string>
#include <vector>

struct MeshRepairReport {
  std::size_t numMergedVertices = 0;
  std::size_t numInvalidTriangles = 0;     ///< non-finite coordinates
  std::size_t numDegenerateTriangles = 0;  ///< collapsed by vertex merging
  std::size_t numDuplicateTriangles = 0;
  std::size_t numFlippedTriangles = 0;     ///< made consistent with neighbors
  std::size_t numInvertedComponents = 0;   ///< closed parts turned outwards
  std::size_t numFilledHoles = 0;
  std::size_t numHoleTriangles = 0;        ///< triangles added to fill holes
  std::size_t numUnusedVertices = 0;
  /// left after the repair
  std::size_t numBoundaryEdges = 0;
  std::size_t numNonManifoldEdges = 0;

  bool Changed() const {
    return numMergedVertices || numInvalidTriangles ||
           numDegenerateTriangles || numDuplicateTriangles ||
           numFlippedTriangles || numInvertedComponents || numFilledHoles ||
           numUnusedVertices;
  }
  /// short human readable list of the repairs (empty if nothing changed)
  std::string Summary() const;
};

/// Repairs the mesh in place. Vertices closer than tolerance * bounding box
/// diagonal are merged. Holes are only filled if fillHoles is set.
DLL_EXPORT MeshRepairReport RepairSurfaceMesh(std::vector<Vec3f> &verts,
                                              std::vector<Triangle> &tris,
                                              bool fillHoles = true,
                                              double tolerance = 1e-6);

#endif  // MESH_REPAIR_H
