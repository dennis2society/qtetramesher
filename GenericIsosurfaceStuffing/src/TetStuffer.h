/*
 * TetStuffer.h
 *
 * Main coordinator for the Isosurface Stuffing algorithm.
 * Orchestrates: BCC lattice → SDF evaluation → Warping → Tet extraction →
 * feature snapping and quality optimization.
 */

#ifndef TET_STUFFER_H
#define TET_STUFFER_H

#include "TetraMeshTools/GeometryTypes.h"
#include <vector>

class TetStuffer {
public:
  /**
   * Run the isosurface stuffing pipeline.
   *
   * @param surfVerts   Surface mesh vertices
   * @param surfTris    Surface mesh triangles
   * @param gridSpacing BCC lattice spacing
   * @param alphaShort  Short-edge warp threshold
   * @param alphaLong   Long-edge warp threshold
   * @param grading     0 = uniform lattice, (0, 1] = graded, higher values
   *                    give larger interior and flat-surface tetrahedra
   * @param featureAngle  Sharp edge angle in degrees (0 = no feature snapping)
   * @param featureSnap   0..1, how far boundary vertices may move onto sharp
   *                      edges (fraction of the local boundary edge length)
   * @param outVerts    [out] Generated tetra mesh vertices
   * @param outTets     [out] Generated tetrahedra
   */
  static void stuff(const std::vector<Vec3f> &surfVerts,
                    const std::vector<Triangle> &surfTris,
                    float gridSpacing,
                    float alphaShort,
                    float alphaLong,
                    float grading,
                    float featureAngle,
                    float featureSnap,
                    std::vector<Vec3f> &outVerts,
                    std::vector<Tetrahedron> &outTets);
};

#endif // TET_STUFFER_H
