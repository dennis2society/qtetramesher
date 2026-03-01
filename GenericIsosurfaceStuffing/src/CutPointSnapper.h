/*
 * CutPointSnapper.h
 *
 * After warping, some lattice tets still have edges crossing the isosurface
 * with un-snapped cut points. This class handles the subdivision of those
 * tets by inserting cut-point vertices and re-tetrahedralizing.
 *
 * Cut points are projected onto the actual surface mesh for better
 * surface conformance.
 */

#ifndef CUT_POINT_SNAPPER_H
#define CUT_POINT_SNAPPER_H

#include "BCCLattice.h"
#include "SignedDistanceField.h"
#include "Warp.h"
#include <vector>
#include <cstdint>
#include <unordered_map>

class CutPointSnapper {
public:
  /**
   * Given a lattice with SDF values and the warping results, produce
   * the set of interior output tets. Each lattice tet is classified:
   *   - All vertices inside (SDF <= 0): emit directly
   *   - All vertices outside (SDF > 0): discard
   *   - Mixed: use cut points to subdivide, emit interior sub-tets
   *
   * Cut points are projected onto the surface mesh for accuracy.
   */
  static void extractInteriorTets(
      BCCLattice &lattice,
      Warp &warp,
      const SignedDistanceField &sdf,
      std::vector<Vec3f> &outVertices,
      std::vector<Tetrahedron> &outTetras);

private:
  static int classifyTet(const BCCLattice &lattice,
                          const std::array<unsigned int, 4> &tet);

  static unsigned int findOrCreateCutVertex(
      unsigned int v0, unsigned int v1,
      const BCCLattice &lattice,
      const Warp &warp,
      const SignedDistanceField &sdf,
      std::unordered_map<uint64_t, unsigned int> &edgeCutMap,
      std::unordered_map<unsigned int, unsigned int> &vertexMap,
      std::vector<Vec3f> &outVertices);

  static uint64_t edgeKey(unsigned int v0, unsigned int v1);
};

#endif // CUT_POINT_SNAPPER_H
