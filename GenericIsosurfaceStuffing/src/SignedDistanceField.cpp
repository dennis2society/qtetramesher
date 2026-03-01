/*
 * SignedDistanceField.cpp
 *
 * Robust signed distance field using:
 *   - Baerentzen & Aanaes pseudo-normal method for sign (SIGGRAPH 2005)
 *   - Feature classification: tracks whether closest point is on a face
 *     interior, edge, or vertex, and uses the appropriate pseudo-normal
 *   - Uniform grid spatial hash for O(1) closest-triangle queries
 */

#include "SignedDistanceField.h"
#include <limits>
#include <algorithm>
#include <cmath>
#include <cstdio>

// ============================================================================
// Helpers
// ============================================================================

uint64_t SignedDistanceField::makeEdgeKey(unsigned int v0, unsigned int v1) {
  unsigned int lo = std::min(v0, v1);
  unsigned int hi = std::max(v0, v1);
  return ((uint64_t)lo << 32) | (uint64_t)hi;
}

int SignedDistanceField::gridCellIndex(int ix, int iy, int iz) const {
  return iz * gx_ * gy_ + iy * gx_ + ix;
}

// ============================================================================
// Construction & precomputation
// ============================================================================

SignedDistanceField::SignedDistanceField(const std::vector<Vec3f> &vertices,
                                        const std::vector<Triangle> &triangles)
    : verts_(vertices), tris_(triangles) {
  precompute();
  buildGrid();
}

void SignedDistanceField::precompute() {
  // --- Face normals (unit length) ---
  faceNormals_.resize(tris_.size());
  for (size_t i = 0; i < tris_.size(); ++i) {
    const Vec3f &v0 = verts_[tris_[i].index[0]];
    const Vec3f &v1 = verts_[tris_[i].index[1]];
    const Vec3f &v2 = verts_[tris_[i].index[2]];
    Vec3f fn = (v1 - v0).cross(v2 - v0);
    float len = fn.length();
    if (len > 1e-15f) fn = fn / len;
    faceNormals_[i] = fn;
  }

  // --- Vertex pseudo-normals (angle-weighted sum of face normals) ---
  vertexPseudoNormals_.resize(verts_.size(), Vec3f(0.0f, 0.0f, 0.0f));
  for (size_t i = 0; i < tris_.size(); ++i) {
    const Triangle &tri = tris_[i];
    const Vec3f &v0 = verts_[tri.index[0]];
    const Vec3f &v1 = verts_[tri.index[1]];
    const Vec3f &v2 = verts_[tri.index[2]];

    // Compute interior angle at each vertex
    Vec3f e01 = v1 - v0; float l01 = e01.length();
    Vec3f e02 = v2 - v0; float l02 = e02.length();
    Vec3f e12 = v2 - v1; float l12 = e12.length();
    Vec3f e10 = v0 - v1; float l10 = l01;
    Vec3f e20 = v0 - v2; float l20 = l02;
    Vec3f e21 = v1 - v2; float l21 = l12;

    float angle0 = 0.0f, angle1 = 0.0f, angle2 = 0.0f;
    if (l01 > 1e-12f && l02 > 1e-12f) {
      float d = e01.dot(e02) / (l01 * l02);
      angle0 = std::acos(std::max(-1.0f, std::min(1.0f, d)));
    }
    if (l10 > 1e-12f && l12 > 1e-12f) {
      float d = e10.dot(e12) / (l10 * l12);
      angle1 = std::acos(std::max(-1.0f, std::min(1.0f, d)));
    }
    if (l20 > 1e-12f && l21 > 1e-12f) {
      float d = e20.dot(e21) / (l20 * l21);
      angle2 = std::acos(std::max(-1.0f, std::min(1.0f, d)));
    }

    vertexPseudoNormals_[tri.index[0]] += faceNormals_[i] * angle0;
    vertexPseudoNormals_[tri.index[1]] += faceNormals_[i] * angle1;
    vertexPseudoNormals_[tri.index[2]] += faceNormals_[i] * angle2;
  }

  // --- Edge pseudo-normals (sum of adjacent face normals) ---
  for (size_t i = 0; i < tris_.size(); ++i) {
    const Triangle &tri = tris_[i];
    uint64_t k01 = makeEdgeKey(tri.index[0], tri.index[1]);
    uint64_t k12 = makeEdgeKey(tri.index[1], tri.index[2]);
    uint64_t k20 = makeEdgeKey(tri.index[2], tri.index[0]);
    // operator[] default-constructs Vec3f to (0,0,0) if not present
    edgePseudoNormals_[k01] += faceNormals_[i];
    edgePseudoNormals_[k12] += faceNormals_[i];
    edgePseudoNormals_[k20] += faceNormals_[i];
  }
}

// ============================================================================
// Uniform grid spatial hash
// ============================================================================

void SignedDistanceField::buildGrid() {
  if (verts_.empty()) return;

  // Compute bounding box of the surface mesh
  Vec3f bmin = verts_[0], bmax = verts_[0];
  for (size_t i = 1; i < verts_.size(); ++i) {
    if (verts_[i].x < bmin.x) bmin.x = verts_[i].x;
    if (verts_[i].y < bmin.y) bmin.y = verts_[i].y;
    if (verts_[i].z < bmin.z) bmin.z = verts_[i].z;
    if (verts_[i].x > bmax.x) bmax.x = verts_[i].x;
    if (verts_[i].y > bmax.y) bmax.y = verts_[i].y;
    if (verts_[i].z > bmax.z) bmax.z = verts_[i].z;
  }

  Vec3f diag = bmax - bmin;
  float diagonal = diag.length();
  if (diagonal < 1e-10f) diagonal = 1.0f;

  // Cell size: aim for roughly 1-2 triangles per cell on average
  float numTris = std::max(1.0f, (float)tris_.size());
  cellSize_ = diagonal / std::cbrt(numTris);
  cellSize_ = std::max(cellSize_, 1e-8f);

  // Add generous padding so lattice points outside the mesh still find nearby triangles
  float pad = cellSize_ * 5.0f;
  gridMin_ = bmin - Vec3f(pad, pad, pad);
  Vec3f gridMax = bmax + Vec3f(pad, pad, pad);

  gx_ = std::max(1, (int)std::ceil((gridMax.x - gridMin_.x) / cellSize_));
  gy_ = std::max(1, (int)std::ceil((gridMax.y - gridMin_.y) / cellSize_));
  gz_ = std::max(1, (int)std::ceil((gridMax.z - gridMin_.z) / cellSize_));

  // Cap to avoid excessive memory in case of degenerate input
  int maxCells = 512;
  if (gx_ > maxCells) { gx_ = maxCells; cellSize_ = (gridMax.x - gridMin_.x) / gx_; }
  if (gy_ > maxCells) gy_ = maxCells;
  if (gz_ > maxCells) gz_ = maxCells;

  grid_.resize(gx_ * gy_ * gz_);

  // Assign each triangle to all grid cells its bounding box overlaps
  for (unsigned int ti = 0; ti < tris_.size(); ++ti) {
    const Triangle &tri = tris_[ti];
    const Vec3f &v0 = verts_[tri.index[0]];
    const Vec3f &v1 = verts_[tri.index[1]];
    const Vec3f &v2 = verts_[tri.index[2]];

    Vec3f tmin(std::min({v0.x, v1.x, v2.x}),
               std::min({v0.y, v1.y, v2.y}),
               std::min({v0.z, v1.z, v2.z}));
    Vec3f tmax(std::max({v0.x, v1.x, v2.x}),
               std::max({v0.y, v1.y, v2.y}),
               std::max({v0.z, v1.z, v2.z}));

    int ix0 = std::max(0, (int)((tmin.x - gridMin_.x) / cellSize_));
    int iy0 = std::max(0, (int)((tmin.y - gridMin_.y) / cellSize_));
    int iz0 = std::max(0, (int)((tmin.z - gridMin_.z) / cellSize_));
    int ix1 = std::min(gx_ - 1, (int)((tmax.x - gridMin_.x) / cellSize_));
    int iy1 = std::min(gy_ - 1, (int)((tmax.y - gridMin_.y) / cellSize_));
    int iz1 = std::min(gz_ - 1, (int)((tmax.z - gridMin_.z) / cellSize_));

    for (int iz = iz0; iz <= iz1; ++iz)
      for (int iy = iy0; iy <= iy1; ++iy)
        for (int ix = ix0; ix <= ix1; ++ix)
          grid_[gridCellIndex(ix, iy, iz)].push_back(ti);
  }
}

// ============================================================================
// Closest-point on triangle with feature classification
// ============================================================================

Vec3f SignedDistanceField::closestPointOnTriangle(
    const Vec3f &p, unsigned int triIdx,
    float &distSq, Feature &feat) const {
  const Triangle &tri = tris_[triIdx];
  const Vec3f &v0 = verts_[tri.index[0]];
  const Vec3f &v1 = verts_[tri.index[1]];
  const Vec3f &v2 = verts_[tri.index[2]];

  Vec3f ab = v1 - v0;
  Vec3f ac = v2 - v0;
  Vec3f ap = p - v0;

  float d1 = ab.dot(ap);
  float d2 = ac.dot(ap);
  if (d1 <= 0.0f && d2 <= 0.0f) {
    feat = FEAT_VERT_0;
    distSq = (p - v0).squaredLength();
    return v0;
  }

  Vec3f bp = p - v1;
  float d3 = ab.dot(bp);
  float d4 = ac.dot(bp);
  if (d3 >= 0.0f && d4 <= d3) {
    feat = FEAT_VERT_1;
    distSq = (p - v1).squaredLength();
    return v1;
  }

  float vc = d1 * d4 - d3 * d2;
  if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
    feat = FEAT_EDGE_01;
    float v = d1 / (d1 - d3);
    Vec3f pt = v0 + ab * v;
    distSq = (p - pt).squaredLength();
    return pt;
  }

  Vec3f cp = p - v2;
  float d5 = ab.dot(cp);
  float d6 = ac.dot(cp);
  if (d6 >= 0.0f && d5 <= d6) {
    feat = FEAT_VERT_2;
    distSq = (p - v2).squaredLength();
    return v2;
  }

  float vb = d5 * d2 - d1 * d6;
  if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
    feat = FEAT_EDGE_20;
    float w = d2 / (d2 - d6);
    Vec3f pt = v0 + ac * w;
    distSq = (p - pt).squaredLength();
    return pt;
  }

  float va = d3 * d6 - d5 * d4;
  if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
    feat = FEAT_EDGE_12;
    float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
    Vec3f pt = v1 + (v2 - v1) * w;
    distSq = (p - pt).squaredLength();
    return pt;
  }

  // Inside the face
  feat = FEAT_FACE;
  float denom = 1.0f / (va + vb + vc);
  float v = vb * denom;
  float w = vc * denom;
  Vec3f pt = v0 + ab * v + ac * w;
  distSq = (p - pt).squaredLength();
  return pt;
}

// ============================================================================
// Pseudo-normal lookup based on closest feature
// ============================================================================

Vec3f SignedDistanceField::getPseudoNormal(unsigned int triIdx,
                                           Feature feat) const {
  const Triangle &tri = tris_[triIdx];
  switch (feat) {
    case FEAT_FACE:
      return faceNormals_[triIdx];
    case FEAT_VERT_0:
      return vertexPseudoNormals_[tri.index[0]];
    case FEAT_VERT_1:
      return vertexPseudoNormals_[tri.index[1]];
    case FEAT_VERT_2:
      return vertexPseudoNormals_[tri.index[2]];
    case FEAT_EDGE_01: {
      uint64_t key = makeEdgeKey(tri.index[0], tri.index[1]);
      auto it = edgePseudoNormals_.find(key);
      return (it != edgePseudoNormals_.end()) ? it->second : faceNormals_[triIdx];
    }
    case FEAT_EDGE_12: {
      uint64_t key = makeEdgeKey(tri.index[1], tri.index[2]);
      auto it = edgePseudoNormals_.find(key);
      return (it != edgePseudoNormals_.end()) ? it->second : faceNormals_[triIdx];
    }
    case FEAT_EDGE_20: {
      uint64_t key = makeEdgeKey(tri.index[2], tri.index[0]);
      auto it = edgePseudoNormals_.find(key);
      return (it != edgePseudoNormals_.end()) ? it->second : faceNormals_[triIdx];
    }
  }
  return faceNormals_[triIdx];
}

// ============================================================================
// Grid-accelerated closest-point query
// ============================================================================

SignedDistanceField::ClosestResult
SignedDistanceField::findClosest(const Vec3f &p) const {
  ClosestResult best;
  best.distSq = std::numeric_limits<float>::max();
  best.triIdx = 0;
  best.feature = FEAT_FACE;

  int cx = std::max(0, std::min(gx_ - 1, (int)((p.x - gridMin_.x) / cellSize_)));
  int cy = std::max(0, std::min(gy_ - 1, (int)((p.y - gridMin_.y) / cellSize_)));
  int cz = std::max(0, std::min(gz_ - 1, (int)((p.z - gridMin_.z) / cellSize_)));

  int maxR = std::max({gx_, gy_, gz_});

  for (int r = 0; r <= maxR; ++r) {
    // After searching shell r, minimum distance to any unsearched cell is
    // r * cellSize_. If our best is closer, we can stop.
    if (r > 0 && best.distSq < std::numeric_limits<float>::max()) {
      float minOutsideDist = (float)r * cellSize_;
      if (best.distSq <= minOutsideDist * minOutsideDist) break;
    }

    // Search all cells on the Chebyshev shell of radius r
    int x0 = std::max(0, cx - r), x1 = std::min(gx_ - 1, cx + r);
    int y0 = std::max(0, cy - r), y1 = std::min(gy_ - 1, cy + r);
    int z0 = std::max(0, cz - r), z1 = std::min(gz_ - 1, cz + r);

    for (int ix = x0; ix <= x1; ++ix) {
      for (int iy = y0; iy <= y1; ++iy) {
        for (int iz = z0; iz <= z1; ++iz) {
          // Only process cells on the current shell boundary
          if (r > 0) {
            bool onShell = (ix == cx - r || ix == cx + r ||
                            iy == cy - r || iy == cy + r ||
                            iz == cz - r || iz == cz + r);
            if (!onShell) continue;
          }

          const auto &cell = grid_[gridCellIndex(ix, iy, iz)];
          for (unsigned int triIdx : cell) {
            float dSq;
            Feature feat;
            Vec3f cp = closestPointOnTriangle(p, triIdx, dSq, feat);
            if (dSq < best.distSq) {
              best.distSq = dSq;
              best.point = cp;
              best.triIdx = triIdx;
              best.feature = feat;
            }
          }
        }
      }
    }
  }

  return best;
}

// ============================================================================
// Public interface
// ============================================================================

float SignedDistanceField::evaluate(const Vec3f &p) const {
  ClosestResult res = findClosest(p);

  float dist = std::sqrt(res.distSq);
  if (dist < 1e-10f) return 0.0f;

  // Sign from pseudo-normal: positive dot = outside, negative = inside
  Vec3f pn = getPseudoNormal(res.triIdx, res.feature);
  Vec3f diff = p - res.point;
  float dotVal = diff.dot(pn);

  return (dotVal >= 0.0f) ? dist : -dist;
}

Vec3f SignedDistanceField::closestSurfacePoint(const Vec3f &p) const {
  ClosestResult res = findClosest(p);
  return res.point;
}
