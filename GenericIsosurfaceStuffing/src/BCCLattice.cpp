/*
 * BCCLattice.cpp
 *
 * Constructs a Body-Centered Cubic lattice and its canonical tet decomposition.
 *
 * A BCC lattice places vertices at:
 *   - Corner positions: origin + (i, j, k) * spacing
 *   - Body center positions: origin + (i+0.5, j+0.5, k+0.5) * spacing
 *
 * Each cube cell (i,j,k) has 8 corner vertices and 1 body center.
 * The cell is decomposed into 12 tetrahedra: for each of the 6 faces of the
 * cube, we form 2 triangles, each of which joins the body center to make a tet.
 *
 * Actually, the standard BCC decomposition creates 24 tets per cube:
 * each face is split into 4 triangles (by the face diagonals), and each joins
 * the body center. But the canonical approach for isosurface stuffing is:
 *
 * Each cube is split into 12 tets by connecting each face's 2 triangles
 * to the body center. The face diagonal direction alternates with parity.
 */

#include "BCCLattice.h"
#include <cstdio>

BCCLattice::BCCLattice(const Vec3f &bboxMin, const Vec3f &bboxMax, float spacing)
    : spacing_(spacing) {
  // Add padding of 2 cells around the bounding box
  float pad = 2.0f * spacing;
  origin_ = bboxMin - Vec3f(pad, pad, pad);
  Vec3f extent = (bboxMax + Vec3f(pad, pad, pad)) - origin_;

  ni = (int)std::ceil(extent.x / spacing) + 1;
  nj = (int)std::ceil(extent.y / spacing) + 1;
  nk = (int)std::ceil(extent.z / spacing) + 1;

  numCorners_ = ni * nj * nk;
  int numCells = (ni - 1) * (nj - 1) * (nk - 1);

  // Create corner vertices
  vertices.resize(numCorners_ + numCells);
  sdfValues.resize(numCorners_ + numCells, 0.0f);
  isBodyCenter.resize(numCorners_ + numCells, false);

  for (int k = 0; k < nk; ++k) {
    for (int j = 0; j < nj; ++j) {
      for (int i = 0; i < ni; ++i) {
        unsigned int idx = cornerIndex(i, j, k);
        vertices[idx] = Vec3f(origin_.x + i * spacing,
                               origin_.y + j * spacing,
                               origin_.z + k * spacing);
      }
    }
  }

  // Create body center vertices
  for (int k = 0; k < nk - 1; ++k) {
    for (int j = 0; j < nj - 1; ++j) {
      for (int i = 0; i < ni - 1; ++i) {
        unsigned int idx = bodyCenterIndex(i, j, k);
        vertices[idx] = Vec3f(origin_.x + (i + 0.5f) * spacing,
                               origin_.y + (j + 0.5f) * spacing,
                               origin_.z + (k + 0.5f) * spacing);
        isBodyCenter[idx] = true;
      }
    }
  }
}

unsigned int BCCLattice::cornerIndex(int i, int j, int k) const {
  return (unsigned int)(k * ni * nj + j * ni + i);
}

unsigned int BCCLattice::bodyCenterIndex(int i, int j, int k) const {
  return (unsigned int)(numCorners_ + k * (ni - 1) * (nj - 1) + j * (ni - 1) + i);
}

void BCCLattice::buildTets() {
  tets.clear();
  // Reserve approximate space: 12 tets per cell
  int numCells = (ni - 1) * (nj - 1) * (nk - 1);
  tets.reserve(numCells * 12);

  for (int k = 0; k < nk - 1; ++k) {
    for (int j = 0; j < nj - 1; ++j) {
      for (int i = 0; i < ni - 1; ++i) {
        // The 8 corners of the cube cell (i,j,k)
        unsigned int c000 = cornerIndex(i, j, k);
        unsigned int c100 = cornerIndex(i + 1, j, k);
        unsigned int c010 = cornerIndex(i, j + 1, k);
        unsigned int c110 = cornerIndex(i + 1, j + 1, k);
        unsigned int c001 = cornerIndex(i, j, k + 1);
        unsigned int c101 = cornerIndex(i + 1, j, k + 1);
        unsigned int c011 = cornerIndex(i, j + 1, k + 1);
        unsigned int c111 = cornerIndex(i + 1, j + 1, k + 1);

        // Body center
        unsigned int bc = bodyCenterIndex(i, j, k);

        // Decompose each face into 2 triangles, each connecting to the body
        // center. The diagonal direction on each face follows the "parity"
        // convention to ensure consistent orientation.
        // Parity determines the diagonal direction on each face.
        int parity = (i + j + k) % 2;

        if (parity == 0) {
          // Bottom face (z=k): c000, c100, c010, c110
          // Diagonal: c000-c110
          tets.push_back({c000, c100, c110, bc});
          tets.push_back({c000, c110, c010, bc});

          // Top face (z=k+1): c001, c101, c011, c111
          // Diagonal: c001-c111
          tets.push_back({c001, c101, c111, bc});
          tets.push_back({c001, c111, c011, bc});

          // Front face (y=j): c000, c100, c001, c101
          // Diagonal: c000-c101
          tets.push_back({c000, c100, c101, bc});
          tets.push_back({c000, c101, c001, bc});

          // Back face (y=j+1): c010, c110, c011, c111
          // Diagonal: c010-c111
          tets.push_back({c010, c110, c111, bc});
          tets.push_back({c010, c111, c011, bc});

          // Left face (x=i): c000, c010, c001, c011
          // Diagonal: c000-c011
          tets.push_back({c000, c010, c011, bc});
          tets.push_back({c000, c011, c001, bc});

          // Right face (x=i+1): c100, c110, c101, c111
          // Diagonal: c100-c111
          tets.push_back({c100, c110, c111, bc});
          tets.push_back({c100, c111, c101, bc});
        } else {
          // Opposite diagonal direction for odd parity cells
          // Bottom face: diagonal c100-c010
          tets.push_back({c000, c100, c010, bc});
          tets.push_back({c100, c110, c010, bc});

          // Top face: diagonal c101-c011
          tets.push_back({c001, c101, c011, bc});
          tets.push_back({c101, c111, c011, bc});

          // Front face: diagonal c100-c001
          tets.push_back({c000, c100, c001, bc});
          tets.push_back({c100, c101, c001, bc});

          // Back face: diagonal c110-c011
          tets.push_back({c010, c110, c011, bc});
          tets.push_back({c110, c111, c011, bc});

          // Left face: diagonal c010-c001
          tets.push_back({c000, c010, c001, bc});
          tets.push_back({c010, c011, c001, bc});

          // Right face: diagonal c110-c101
          tets.push_back({c100, c110, c101, bc});
          tets.push_back({c110, c111, c101, bc});
        }
      }
    }
  }
}
