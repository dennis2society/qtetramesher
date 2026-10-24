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
 * RepairSurfaceMeshGeogram() also handles those using geogram.
 */

#ifndef MESH_REPAIR_H
#define MESH_REPAIR_H

#include <TetraMeshTools/GeometryTypes.h>
#include <TetraMeshTools/TetraToolsExports.h>

#include <cstddef>
#include <string>
#include <vector>

struct DLL_EXPORT MeshRepairReport {
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

/// Options of RepairSurfaceMeshGeogram().
struct DLL_EXPORT GeogramRepairOptions {
  bool removeSmallParts = true;
  /// parts below this fraction of the total surface area are removed
  double minPartArea = 0.001;
  bool fillHoles = true;
  /// splits intersecting triangles (exact arithmetic) and keeps only the
  /// outer hull, internal parts are removed
  bool resolveIntersections = true;
  /// isotropic remeshing, rounds sharp edges and loses small details
  bool remesh = false;
  std::size_t remeshVertices = 10000;
  double tolerance = 1e-6;  ///< see RepairSurfaceMesh()
};

struct DLL_EXPORT GeogramRepairReport {
  std::size_t verticesBefore = 0;
  std::size_t trianglesBefore = 0;
  std::size_t verticesAfter = 0;
  std::size_t trianglesAfter = 0;
  std::size_t removedParts = 0;
  /// intersecting triangles were split or inner parts removed
  bool intersectionsResolved = false;
  bool remeshed = false;
  /// RepairSurfaceMesh() run at the end (orientation, remaining defects)
  MeshRepairReport final;

  std::string Summary() const;
};

/// Repairs defects RepairSurfaceMesh() cannot fix (self-intersections,
/// non-manifold edges, small parts) with geogram, optionally remeshes the
/// surface. The mesh is only replaced on success. Throws std::runtime_error
/// on failure.
DLL_EXPORT GeogramRepairReport
RepairSurfaceMeshGeogram(std::vector<Vec3f> &verts, std::vector<Triangle> &tris,
                         const GeogramRepairOptions &options);

#endif  // MESH_REPAIR_H
