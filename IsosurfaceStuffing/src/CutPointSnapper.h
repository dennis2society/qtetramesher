/*
 * CutPointSnapper.h
 *
 * After warping, some lattice tets still have edges crossing the isosurface
 * with un-snapped cut points. This class handles the subdivision of those
 * tets by inserting cut-point vertices and re-tetrahedralizing.
 */

#ifndef CUT_POINT_SNAPPER_H
#define CUT_POINT_SNAPPER_H

#include "BCCLattice.h"
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
   */
  static void extractInteriorTets(
      BCCLattice &lattice,
      Warp &warp,
      std::vector<Vec3f> &outVertices,
      std::vector<Tetrahedron> &outTetras);

private:
  /**
   * Classify a tet's vertices as inside (SDF <= 0) or outside.
   * Returns a 4-bit mask where bit i corresponds to vertex i.
   * Bit = 1 means "inside or on surface".
   */
  static int classifyTet(const BCCLattice &lattice,
                          const std::array<unsigned int, 4> &tet);

  /**
   * Find the cut point index on the edge between v0 and v1.
   * Looks up previously computed cut points.
   * If a vertex was warped to the surface, it serves as its own cut point.
   * Returns the output vertex index for the cut point.
   */
  static unsigned int findOrCreateCutVertex(
      unsigned int v0, unsigned int v1,
      const BCCLattice &lattice,
      const Warp &warp,
      std::unordered_map<uint64_t, unsigned int> &edgeCutMap,
      std::vector<Vec3f> &outVertices);

  static uint64_t edgeKey(unsigned int v0, unsigned int v1);
};

#endif // CUT_POINT_SNAPPER_H
