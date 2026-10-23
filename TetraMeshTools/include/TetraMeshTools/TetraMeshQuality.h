/*
 * TetraMeshQuality.h
 *
 * Quality measurements of a tetrahedral mesh (element shape, volume
 * compared to the surface mesh it was generated from).
 */

#ifndef TETRAMESHTOOLS_TETRAMESHQUALITY_H_
#define TETRAMESHTOOLS_TETRAMESHQUALITY_H_

#include <cstddef>
#include <vector>

#include "GeometryTypes.h"
#include "TetraToolsExports.h"

namespace TetraTools {

struct TetraMeshQuality {
  std::size_t numTetras = 0;
  std::size_t numVertices = 0;
  double tetraVolume = 0.0;
  /// enclosed volume of the surface mesh, negative if there is none
  double surfaceVolume = -1.0;
  /// 6*sqrt(2)*V / l_rms^3: 1 for a regular tetrahedron, 0 for a flat one
  double minQuality = 0.0;
  double meanQuality = 0.0;
  std::size_t numPoorTetras = 0;  ///< quality below kPoorQuality
  std::size_t numInvertedTetras = 0;
  double minDihedralDeg = 0.0;
  double maxDihedralDeg = 0.0;
  double minEdgeLength = 0.0;
  double maxEdgeLength = 0.0;

  static constexpr double kPoorQuality = 0.1;
};

/// Enclosed volume of a closed, outward oriented triangle mesh.
DLL_EXPORT double ComputeSurfaceVolume(const std::vector<Vec3f> &verts,
                                       const std::vector<Triangle> &tris);

/// surfVerts/surfTris may be empty; surfaceVolume is negative then.
DLL_EXPORT TetraMeshQuality ComputeTetraMeshQuality(
    const std::vector<Vec3f> &verts, const std::vector<Tetrahedron> &tetras,
    const std::vector<Vec3f> &surfVerts, const std::vector<Triangle> &surfTris);

}  // namespace TetraTools

#endif /* TETRAMESHTOOLS_TETRAMESHQUALITY_H_ */
