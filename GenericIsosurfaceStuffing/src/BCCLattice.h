/*
 * BCCLattice.h
 *
 * Background mesh for isosurface stuffing.
 *
 * The domain is covered by axis-aligned cells (one uniform grid, or a 2:1
 * balanced octree when grading is requested). Each cell contributes a lattice
 * point at its centre, each cell corner is a lattice point too, and the
 * tetrahedra are the pyramids from the two cell centres sharing a face to the
 * four edges of that face. For a uniform grid this is exactly the BCC lattice
 * tetrahedralization used by Labelle & Shewchuk; for the octree the same
 * construction stays conforming across level transitions.
 */

#ifndef BCC_LATTICE_H
#define BCC_LATTICE_H

#include <TetraMeshTools/Vec3d.h>

#include <vector>

class SignedDistanceField;

struct BccTet {
  int v[4];
};

class BCCLattice {
public:
  /**
   * @param bbMin,bbMax  Bounding box of the surface to cover.
   * @param h            Finest cell size.
   * @param graded       Coarsen cells away from the surface.
   * @param sdf          Used to locate the surface when grading.
   */
  void build(const Vec3d &bbMin, const Vec3d &bbMax, double h, bool graded,
             const SignedDistanceField &sdf);

  const std::vector<Vec3d> &positions() const { return positions_; }
  const std::vector<BccTet> &tets() const { return tets_; }
  /// True for cell-centre lattice points, false for cell corners.
  bool isCenter(int v) const { return isCenter_[v] != 0; }
  std::size_t cellCount() const { return cellCount_; }

private:
  std::vector<Vec3d> positions_;
  std::vector<unsigned char> isCenter_;
  std::vector<BccTet> tets_;
  std::size_t cellCount_ = 0;
};

#endif  // BCC_LATTICE_H
