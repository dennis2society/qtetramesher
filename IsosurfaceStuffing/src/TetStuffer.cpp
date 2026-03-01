/*
 * TetStuffer.cpp
 *
 * Orchestrates the Isosurface Stuffing pipeline:
 *
 * 1. Compute bounding box of the surface mesh.
 * 2. Build a BCC lattice covering the bounding box (with padding).
 * 3. Evaluate the signed distance field at every lattice vertex.
 * 4. Build the BCC tet decomposition.
 * 5. Apply Shewchuk's warping rules to snap near-surface vertices.
 * 6. Extract interior tetrahedra (with cut-based subdivision for mixed tets).
 */

#include "TetStuffer.h"
#include "BCCLattice.h"
#include "SignedDistanceField.h"
#include "Warp.h"
#include "CutPointSnapper.h"

#include <iostream>
#include <limits>
#include <cmath>

void TetStuffer::stuff(const std::vector<Vec3f> &surfVerts,
                       const std::vector<Triangle> &surfTris,
                       float gridSpacing,
                       float alphaShort,
                       float alphaLong,
                       std::vector<Vec3f> &outVerts,
                       std::vector<Tetrahedron> &outTets) {

  outVerts.clear();
  outTets.clear();

  if (surfVerts.empty() || surfTris.empty()) {
    std::cout << "IsosurfaceStuffing: empty input\n";
    return;
  }

  // 1. Compute bounding box
  Vec3f bmin = surfVerts[0];
  Vec3f bmax = surfVerts[0];
  for (size_t i = 1; i < surfVerts.size(); ++i) {
    if (surfVerts[i].x < bmin.x) bmin.x = surfVerts[i].x;
    if (surfVerts[i].y < bmin.y) bmin.y = surfVerts[i].y;
    if (surfVerts[i].z < bmin.z) bmin.z = surfVerts[i].z;
    if (surfVerts[i].x > bmax.x) bmax.x = surfVerts[i].x;
    if (surfVerts[i].y > bmax.y) bmax.y = surfVerts[i].y;
    if (surfVerts[i].z > bmax.z) bmax.z = surfVerts[i].z;
  }

  std::cout << "IsosurfaceStuffing: BBox [" << bmin.x << "," << bmin.y << "," << bmin.z
            << "] - [" << bmax.x << "," << bmax.y << "," << bmax.z << "]\n";
  std::cout << "IsosurfaceStuffing: Grid spacing = " << gridSpacing << "\n";

  // 2. Build BCC lattice
  BCCLattice lattice(bmin, bmax, gridSpacing);
  std::cout << "IsosurfaceStuffing: Lattice " << lattice.ni << " x " << lattice.nj
            << " x " << lattice.nk << " (" << lattice.vertices.size() << " vertices)\n";

  // 3. Evaluate signed distance field at every lattice vertex
  SignedDistanceField sdf(surfVerts, surfTris);
  for (size_t i = 0; i < lattice.vertices.size(); ++i) {
    lattice.sdfValues[i] = sdf.evaluate(lattice.vertices[i]);
  }
  std::cout << "IsosurfaceStuffing: SDF evaluated at " << lattice.vertices.size() << " vertices\n";

  // 4. Build BCC tet decomposition
  lattice.buildTets();
  std::cout << "IsosurfaceStuffing: " << lattice.tets.size() << " lattice tetrahedra\n";

  // 5. Apply warping rules
  Warp warp(lattice, sdf, alphaShort, alphaLong);
  std::cout << "IsosurfaceStuffing: " << warp.warpedVertices.size() << " vertices warped, "
            << warp.remainingCutPoints.size() << " remaining cut points\n";

  // 6. Extract interior tetrahedra
  CutPointSnapper::extractInteriorTets(lattice, warp, sdf, outVerts, outTets);
  std::cout << "IsosurfaceStuffing: Output: " << outVerts.size() << " vertices, "
            << outTets.size() << " tetrahedra\n";
}
