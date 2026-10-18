/*
 * SurfaceChecker.h
 *
 * Validates a triangle surface mesh for use as input of volumetric
 * mesh generators (Tetgen, GMSH, ...): self-intersections, degenerate
 * triangles, open boundaries and non-manifold edges.
 */

#ifndef SURFACE_CHECKER_H
#define SURFACE_CHECKER_H

#include <TetraMeshTools/GeometryTypes.h>
#include <TetraMeshTools/TetraToolsExports.h>

#include <atomic>
#include <cstddef>
#include <utility>
#include <vector>

struct SurfaceCheckResult {
  std::size_t numTriangles = 0;
  std::size_t numIntersectingPairs = 0;
  std::size_t numIntersectingTriangles = 0;
  std::size_t numDegenerateTriangles = 0;
  std::size_t numBoundaryEdges = 0;
  std::size_t numNonManifoldEdges = 0;
  /// Edges traversed in the same direction twice (inconsistent orientation).
  std::size_t numInconsistentEdges = 0;
  std::size_t numUnusedVertices = 0;
  std::size_t numComponents = 0;
  /// Sum of component genera; -1 if the surface is not closed and manifold.
  int genus = -1;
  /// First few intersecting triangle index pairs, for diagnostics.
  std::vector<std::pair<unsigned int, unsigned int>> examplePairs;
  /// true if the check was stopped via the cancel flag (results incomplete)
  bool canceled = false;

  bool IsSuitable() const {
    return numIntersectingPairs == 0 && numDegenerateTriangles == 0 &&
           numBoundaryEdges == 0 && numNonManifoldEdges == 0 &&
           numInconsistentEdges == 0;
  }
};

/// The optional cancel flag may be set from another thread to stop the check
/// early (the result then has canceled = true).
DLL_EXPORT SurfaceCheckResult CheckSurfaceMesh(
    const std::vector<Vec3f> &verts, const std::vector<Triangle> &tris,
    const std::atomic<bool> *cancel = nullptr);

#endif  // SURFACE_CHECKER_H
