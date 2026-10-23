/*
 * MeshOptimizer.h
 *
 * Post-processing of the stuffed tetrahedral mesh, without changing its
 * connectivity:
 *  - Feature snapping: isosurface stuffing only places boundary vertices
 *    where lattice edges cross the surface, so sharp edges and corners of the
 *    input are cut off. Boundary vertices close to them are moved onto them.
 *  - Quality smoothing: vertices of poorly shaped tetrahedra are relaxed
 *    (interior vertices freely, boundary vertices along the surface).
 *  - Cap removal: flat tetrahedra lying on the surface with two or three
 *    boundary faces are removed if the boundary stays manifold.
 * Every move is only kept if all tetrahedra around the vertex stay positively
 * oriented and do not get worse than a quality floor, so the mesh stays valid.
 */

#ifndef MESH_OPTIMIZER_H
#define MESH_OPTIMIZER_H

#include <TetraMeshTools/GeometryTypes.h>
#include <TetraMeshTools/Vec3d.h>

#include <array>
#include <cstddef>
#include <vector>

class SignedDistanceField;

struct MeshOptimizerStats {
  std::size_t featureEdges = 0;
  std::size_t featureCorners = 0;
  std::size_t snapCandidates = 0;
  std::size_t snapped = 0;
  std::size_t smoothed = 0;
  std::size_t capsRemoved = 0;
  double minQualityBefore = 0.0;
  double minQualityAfter = 0.0;
};

/**
 * @param surfVerts,surfTris  Input surface (welded).
 * @param sdf                 Distance field of the input surface.
 * @param featureAngleDeg     Edges whose face normals differ by more than this
 *                            are sharp (0 = no feature handling).
 * @param snapStrength        0..1: snap radius as a fraction of the mean
 *                            boundary edge length at the vertex (0 = off).
 * @param pos                 [in/out] Vertex positions.
 * @param tets                [in/out] Positively oriented tetrahedra; flat
 *                            boundary caps are removed.
 */
MeshOptimizerStats optimizeStuffedMesh(const std::vector<Vec3f> &surfVerts,
                                       const std::vector<Triangle> &surfTris,
                                       const SignedDistanceField &sdf,
                                       double featureAngleDeg, double snapStrength,
                                       std::vector<Vec3d> &pos,
                                       std::vector<std::array<int, 4>> &tets);

#endif  // MESH_OPTIMIZER_H
