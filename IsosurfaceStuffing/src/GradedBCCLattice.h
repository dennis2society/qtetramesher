/*
 * GradedBCCLattice.h
 *
 * Builds a graded (multi-resolution) BCC lattice using octree refinement,
 * as suggested in Shewchuk's Isosurface Stuffing paper.
 *
 * Near the surface:  fine cells (size = fineSpacing)
 * Deep interior:     coarse cells (size = fineSpacing * 2^maxDepthDiff)
 *
 * The octree is 2:1 balanced so adjacent cells differ by at most a factor
 * of 2 in size.  Transition faces between different-sized cells produce
 * 8 tetrahedra (instead of 2) to maintain a conforming mesh.
 *
 * The output is stored directly into a BCCLattice object (vertices,
 * sdfValues, tets, isBodyCenter) so the downstream Warp and
 * CutPointSnapper pipeline works unchanged.
 */

#ifndef GRADED_BCC_LATTICE_H
#define GRADED_BCC_LATTICE_H

#include "BCCLattice.h"
#include "SignedDistanceField.h"
#include <vector>
#include <unordered_map>
#include <cstdint>

class GradedBCCLattice {
public:
  /**
   * Build a graded BCC lattice and store the result in @p out.
   *
   * @param bboxMin       Min corner of surface bounding box
   * @param bboxMax       Max corner of surface bounding box
   * @param fineSpacing   Grid spacing for finest cells (near surface)
   * @param maxDepthDiff  Max grading levels (0=uniform, 1=2x, 2=4x ratio)
   * @param sdf           Signed distance field (for refinement + evaluation)
   * @param out           [out] BCCLattice filled with graded mesh data
   */
  static void build(const Vec3f &bboxMin, const Vec3f &bboxMax,
                    float fineSpacing, int maxDepthDiff,
                    const SignedDistanceField &sdf,
                    BCCLattice &out);

private:
  struct OctreeCell {
    Vec3f min;
    float size;
    int depth;
    int children[8]; // indices into cells vector; -1 = no child (leaf)
    bool isLeaf;
  };

  static void subdivide(std::vector<OctreeCell> &cells, int idx);

  static void refineOctree(std::vector<OctreeCell> &cells, int idx,
                           int maxDepth, int minDepth,
                           const SignedDistanceField &sdf);

  static void balanceOctree(std::vector<OctreeCell> &cells);

  /**
   * Find the leaf cell containing @p point.  Returns -1 if outside the root.
   */
  static int findLeaf(const std::vector<OctreeCell> &cells,
                      const Vec3f &point);

  /**
   * Quantise a position into a 64-bit hash key for vertex deduplication.
   * All lattice positions are multiples of halfStep = fineSpacing / 2.
   */
  static uint64_t quantizePos(const Vec3f &pos, const Vec3f &origin,
                              float halfStep);
};

#endif // GRADED_BCC_LATTICE_H
