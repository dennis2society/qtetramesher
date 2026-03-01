/*
 * BCCLattice.h
 *
 * Generates a Body-Centered Cubic (BCC) lattice that covers a given bounding
 * box with specified grid spacing. The BCC lattice has vertices at grid corners
 * and at body centers (half-step offset). Each cube cell is subdivided into
 * tetrahedra using the standard BCC decomposition.
 *
 * Shewchuk's Isosurface Stuffing uses a BCC lattice because:
 * - All resulting tetrahedra have bounded dihedral angles ~71°–109°
 * - The lattice is well-suited for warping operations
 */

#ifndef BCC_LATTICE_H
#define BCC_LATTICE_H

#include "TetraMeshTools/GeometryTypes.h"
#include <vector>
#include <unordered_map>
#include <cmath>
#include <array>

struct BCCEdge {
  unsigned int v0, v1;
};

/**
 * Stores the BCC lattice: corner and body-center vertices, their SDF values,
 * and the canonical tet decomposition of each cell.
 */
class BCCLattice {
public:
  /**
   * Default constructor (for graded lattice builder).
   */
  BCCLattice() : ni(0), nj(0), nk(0), spacing_(0.0f), numCorners_(0) {}

  /**
   * Build a BCC lattice covering (bboxMin - padding) to (bboxMax + padding).
   * @param bboxMin    Min corner of the surface bounding box
   * @param bboxMax    Max corner of the surface bounding box
   * @param spacing    Grid spacing (edge length of cubes)
   */
  BCCLattice(const Vec3f &bboxMin, const Vec3f &bboxMax, float spacing);

  // All lattice vertices (corners first, then body centers)
  std::vector<Vec3f> vertices;

  // SDF value at each vertex (filled externally)
  std::vector<float> sdfValues;

  // Whether each vertex is a body center (true) or corner (false)
  std::vector<bool> isBodyCenter;

  // Tetrahedra of the BCC decomposition (indices into vertices)
  // Each BCC cube produces 24 tetrahedra (from 6 pyramids, each split into 4 tets)
  // Actually, each cube produces exactly 24 tets through center subdivision
  std::vector<std::array<unsigned int, 4>> tets;

  // Grid dimensions
  int ni, nj, nk; // number of corner vertices along each axis
  float spacing_;
  Vec3f origin_;

  /**
   * Get corner vertex index for grid position (i, j, k)
   */
  unsigned int cornerIndex(int i, int j, int k) const;

  /**
   * Get body center vertex index for cell (i, j, k)
   * Cell (i,j,k) spans from corner (i,j,k) to corner (i+1,j+1,k+1)
   */
  unsigned int bodyCenterIndex(int i, int j, int k) const;

  /**
   * Build the BCC tet decomposition after vertices are placed.
   */
  void buildTets();

private:
  int numCorners_;
};

#endif // BCC_LATTICE_H
