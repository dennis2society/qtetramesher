/*
 * IsosurfaceStuffing.h
 *
 *  A self-contained implementation of Jonathan Shewchuk's Isosurface Stuffing
 *  algorithm (http://www.cs.berkeley.edu/~jrs/papers/stuffing.pdf).
 *  No external dependencies — pure C++ using TetraMeshTools types.
 *
 *  The algorithm:
 *  1. Build a Body-Centered Cubic (BCC) lattice covering the bounding box.
 *  2. Evaluate a signed distance field on every lattice vertex.
 *  3. Cut edges that cross the isosurface (sign change) and compute cut points.
 *  4. Apply warping rules (snap near-surface lattice vertices to the surface)
 *     to avoid badly shaped tetrahedra.
 *  5. Subdivide BCC cells into tetrahedra and classify them as inside/outside.
 *  6. Output the inside tetrahedra.
 *
 *  Created 2026
 */

#ifndef ISOSURFACE_STUFFING_H
#define ISOSURFACE_STUFFING_H

#include "TetraMeshTools/GeometryTypes.h"
#include "TetraMeshTools/TetraToolsExports.h"
#include <vector>

class DLL_EXPORT IsosurfaceStuffing {
public:
  IsosurfaceStuffing() {}

  /**
   * Generate a tetrahedral mesh from a triangle surface.
   *
   * @param tris        Input surface triangles (indices into verts)
   * @param verts       Input surface vertices
   * @param gridSpacing Size of the BCC lattice cell
   * @param alphaShort  Short-edge warp threshold (Shewchuk's alpha_s, default ~0.4)
   * @param alphaLong   Long-edge warp threshold (Shewchuk's alpha_l, default ~0.27)
   */
  void GenerateFromSurface(const std::vector<Triangle> &tris,
                           const std::vector<Vec3f> &verts,
                           float gridSpacing,
                           float alphaShort = 0.4f,
                           float alphaLong = 0.27f);

  std::vector<Vec3f> &GetTetraVertices();
  std::vector<Tetrahedron> &GetTetras();

private:
  std::vector<Vec3f> tetraPoints;
  std::vector<Tetrahedron> tetraIndices;
};

#endif // ISOSURFACE_STUFFING_H
