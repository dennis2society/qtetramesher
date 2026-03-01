/*
 * GradedBCCLattice.cpp
 *
 * Octree-based graded BCC lattice builder.
 *
 * 1.  Compute a power-of-2 root cube covering the padded bounding box.
 * 2.  Recursively refine cells near the surface to the finest level.
 * 3.  Enforce 2:1 balance so adjacent leaves differ by at most 1 level.
 * 4.  Place corner and body-center vertices for each leaf cell
 *     (corners are shared/deduplicated via spatial hashing).
 * 5.  Generate BCC tetrahedra per-leaf, using standard 2-tet faces for
 *     same-size / coarser neighbours and 8-tet transition faces for
 *     finer neighbours.
 * 6.  Evaluate the SDF at every vertex.
 */

#include "GradedBCCLattice.h"
#include <cmath>
#include <iostream>
#include <algorithm>

/* ------------------------------------------------------------------ */
/*  Vertex deduplication key                                          */
/* ------------------------------------------------------------------ */
uint64_t GradedBCCLattice::quantizePos(const Vec3f &pos,
                                        const Vec3f &origin,
                                        float halfStep) {
  int64_t ix = (int64_t)std::round((pos.x - origin.x) / halfStep);
  int64_t iy = (int64_t)std::round((pos.y - origin.y) / halfStep);
  int64_t iz = (int64_t)std::round((pos.z - origin.z) / halfStep);
  // Bias to positive range (21-bit each → up to ~2 M grid positions)
  ix += 1048576;
  iy += 1048576;
  iz += 1048576;
  return ((uint64_t)(ix & 0x1FFFFF) << 42) |
         ((uint64_t)(iy & 0x1FFFFF) << 21) |
          (uint64_t)(iz & 0x1FFFFF);
}

/* ------------------------------------------------------------------ */
/*  Octree helpers                                                    */
/* ------------------------------------------------------------------ */
void GradedBCCLattice::subdivide(std::vector<OctreeCell> &cells, int idx) {
  float halfSize = cells[idx].size * 0.5f;
  int newDepth = cells[idx].depth + 1;
  Vec3f cellMin = cells[idx].min;
  cells[idx].isLeaf = false;

  int firstChild = (int)cells.size();
  cells.reserve(cells.size() + 8);

  for (int c = 0; c < 8; ++c) {
    OctreeCell child;
    child.size = halfSize;
    child.depth = newDepth;
    child.isLeaf = true;
    for (int i = 0; i < 8; ++i) child.children[i] = -1;
    float dx = (c & 1) ? halfSize : 0.0f;
    float dy = (c & 2) ? halfSize : 0.0f;
    float dz = (c & 4) ? halfSize : 0.0f;
    child.min = Vec3f(cellMin.x + dx, cellMin.y + dy, cellMin.z + dz);
    cells.push_back(child);
    cells[idx].children[c] = firstChild + c;
  }
}

void GradedBCCLattice::refineOctree(std::vector<OctreeCell> &cells, int idx,
                                     int maxDepth, int minDepth,
                                     const SignedDistanceField &sdf) {
  if (cells[idx].depth >= maxDepth) return;

  bool shouldRefine = false;
  if (cells[idx].depth < minDepth) {
    shouldRefine = true;
  } else {
    float hs = cells[idx].size * 0.5f;
    Vec3f center(cells[idx].min.x + hs,
                 cells[idx].min.y + hs,
                 cells[idx].min.z + hs);
    float dist = std::fabs(sdf.evaluate(center));
    // Refine if surface could be within 2 cell widths
    if (dist < cells[idx].size * 2.0f) {
      shouldRefine = true;
    }
  }

  if (shouldRefine) {
    subdivide(cells, idx);
    // Save children before recursive calls (vector may reallocate)
    int ch[8];
    for (int c = 0; c < 8; ++c) ch[c] = cells[idx].children[c];
    for (int c = 0; c < 8; ++c) {
      refineOctree(cells, ch[c], maxDepth, minDepth, sdf);
    }
  }
}

void GradedBCCLattice::balanceOctree(std::vector<OctreeCell> &cells) {
  bool changed = true;
  while (changed) {
    changed = false;
    int n = (int)cells.size();
    for (int i = 0; i < n; ++i) {
      if (!cells[i].isLeaf) continue;
      float S = cells[i].size;
      float hs = S * 0.5f;

      // Probe the 6 face neighbours
      for (int face = 0; face < 6; ++face) {
        Vec3f probe(cells[i].min.x + hs,
                    cells[i].min.y + hs,
                    cells[i].min.z + hs);
        float eps = S * 0.01f;
        switch (face) {
          case 0: probe.x = cells[i].min.x - eps;     break; // -X
          case 1: probe.x = cells[i].min.x + S + eps;  break; // +X
          case 2: probe.y = cells[i].min.y - eps;      break; // -Y
          case 3: probe.y = cells[i].min.y + S + eps;  break; // +Y
          case 4: probe.z = cells[i].min.z - eps;      break; // -Z
          case 5: probe.z = cells[i].min.z + S + eps;  break; // +Z
        }
        int nb = findLeaf(cells, probe);
        if (nb < 0) continue;
        if (cells[nb].depth < cells[i].depth - 1) {
          subdivide(cells, nb);
          changed = true;
        }
      }
    }
  }
}

int GradedBCCLattice::findLeaf(const std::vector<OctreeCell> &cells,
                                const Vec3f &point) {
  if (cells.empty()) return -1;
  int idx = 0;
  while (true) {
    const OctreeCell &cell = cells[idx];
    if (point.x < cell.min.x - 1e-6f || point.x > cell.min.x + cell.size + 1e-6f ||
        point.y < cell.min.y - 1e-6f || point.y > cell.min.y + cell.size + 1e-6f ||
        point.z < cell.min.z - 1e-6f || point.z > cell.min.z + cell.size + 1e-6f) {
      return -1;
    }
    if (cell.isLeaf) return idx;
    float hs = cell.size * 0.5f;
    int cx = (point.x >= cell.min.x + hs) ? 1 : 0;
    int cy = (point.y >= cell.min.y + hs) ? 1 : 0;
    int cz = (point.z >= cell.min.z + hs) ? 1 : 0;
    int childIdx = cx + 2 * cy + 4 * cz;
    if (cell.children[childIdx] < 0) return idx; // shouldn't happen
    idx = cell.children[childIdx];
  }
}

/* ------------------------------------------------------------------ */
/*  Main build method                                                 */
/* ------------------------------------------------------------------ */
void GradedBCCLattice::build(const Vec3f &bboxMin, const Vec3f &bboxMax,
                              float fineSpacing, int maxDepthDiff,
                              const SignedDistanceField &sdf,
                              BCCLattice &out) {
  // Padded extents
  float pad = 2.0f * fineSpacing;
  Vec3f origin = bboxMin - Vec3f(pad, pad, pad);
  Vec3f end    = bboxMax + Vec3f(pad, pad, pad);
  Vec3f extent = end - origin;

  float maxExtent = std::max({extent.x, extent.y, extent.z});

  // Root cube: side = fineSpacing * 2^maxDepth  >=  maxExtent
  int maxDepth = (int)std::ceil(std::log2(maxExtent / fineSpacing));
  if (maxDepth < 1) maxDepth = 1;
  float rootSize = fineSpacing * std::pow(2.0f, (float)maxDepth);

  // Centre the root cube over the padded bbox
  Vec3f mid = (origin + end) * 0.5f;
  Vec3f rootMin(mid.x - rootSize * 0.5f,
                mid.y - rootSize * 0.5f,
                mid.z - rootSize * 0.5f);

  int minDepth = maxDepth - maxDepthDiff;
  if (minDepth < 0) minDepth = 0;

  // ---------- 1. Build octree -----------------------------------------
  std::vector<OctreeCell> cells;
  cells.reserve(4096);
  {
    OctreeCell root;
    root.min = rootMin;
    root.size = rootSize;
    root.depth = 0;
    root.isLeaf = true;
    for (int i = 0; i < 8; ++i) root.children[i] = -1;
    cells.push_back(root);
  }

  refineOctree(cells, 0, maxDepth, minDepth, sdf);
  balanceOctree(cells);

  // ---------- 2. Collect leaf cells -----------------------------------
  std::vector<int> leaves;
  leaves.reserve(cells.size());
  for (int i = 0; i < (int)cells.size(); ++i) {
    if (cells[i].isLeaf) leaves.push_back(i);
  }

  std::cout << "GradedBCC: " << leaves.size() << " leaf cells  (octree "
            << cells.size() << " nodes, maxDepth " << maxDepth << ")\n";

  // ---------- 3. Place vertices (deduplicated) -------------------------
  float halfStep = fineSpacing * 0.5f;
  std::unordered_map<uint64_t, unsigned int> vertMap;
  vertMap.reserve(leaves.size() * 4);

  out.vertices.clear();
  out.sdfValues.clear();
  out.tets.clear();
  out.isBodyCenter.clear();
  out.spacing_ = fineSpacing;
  out.origin_ = rootMin;
  out.ni = out.nj = out.nk = 0;

  auto getVertex = [&](const Vec3f &pos, bool isBC) -> unsigned int {
    uint64_t key = quantizePos(pos, rootMin, halfStep);
    auto it = vertMap.find(key);
    if (it != vertMap.end()) return it->second;
    unsigned int idx = (unsigned int)out.vertices.size();
    out.vertices.push_back(pos);
    out.sdfValues.push_back(0.0f);
    out.isBodyCenter.push_back(isBC);
    vertMap[key] = idx;
    return idx;
  };

  // Face information table
  // axis: perpendicular axis (0=X, 1=Y, 2=Z)
  // dir : 0 = negative face, 1 = positive face
  // uAxis, vAxis : tangent axes
  struct FaceInfo { int axis, dir, uAxis, vAxis; };
  const FaceInfo faceInfos[6] = {
    {0, 0, 1, 2}, // -X
    {0, 1, 1, 2}, // +X
    {1, 0, 0, 2}, // -Y
    {1, 1, 0, 2}, // +Y
    {2, 0, 0, 1}, // -Z
    {2, 1, 0, 1}, // +Z
  };

  // ---------- 4. Generate tets per leaf cell ---------------------------
  out.tets.reserve(leaves.size() * 12);

  for (int li = 0; li < (int)leaves.size(); ++li) {
    const OctreeCell &cell = cells[leaves[li]];
    float S  = cell.size;
    float hs = S * 0.5f;

    // 8 corner vertices: corners[dx][dy][dz]
    unsigned int corners[2][2][2];
    for (int dz = 0; dz < 2; ++dz)
      for (int dy = 0; dy < 2; ++dy)
        for (int dx = 0; dx < 2; ++dx)
          corners[dx][dy][dz] = getVertex(
              Vec3f(cell.min.x + dx * S,
                    cell.min.y + dy * S,
                    cell.min.z + dz * S),
              false);

    // Body centre
    unsigned int bc = getVertex(
        Vec3f(cell.min.x + hs, cell.min.y + hs, cell.min.z + hs), true);

    // Cell parity (diagonal direction)
    int ci = (int)std::round((cell.min.x - rootMin.x) / S);
    int cj = (int)std::round((cell.min.y - rootMin.y) / S);
    int ck = (int)std::round((cell.min.z - rootMin.z) / S);
    int cellParity = ((ci + cj + ck) % 2 + 2) % 2; // always 0 or 1

    // Helper: face corner from (face, du, dv)
    auto faceCorner = [&](int face, int du, int dv) -> unsigned int {
      switch (face) {
        case 0: return corners[0][du][dv];  // -X
        case 1: return corners[1][du][dv];  // +X
        case 2: return corners[du][0][dv];  // -Y
        case 3: return corners[du][1][dv];  // +Y
        case 4: return corners[du][dv][0];  // -Z
        case 5: return corners[du][dv][1];  // +Z
        default: return 0;
      }
    };

    // Process each face
    for (int f = 0; f < 6; ++f) {
      const FaceInfo &fi = faceInfos[f];

      // Find face neighbour
      Vec3f probe(cell.min.x + hs, cell.min.y + hs, cell.min.z + hs);
      float eps = S * 0.01f;
      switch (f) {
        case 0: probe.x = cell.min.x - eps;     break;
        case 1: probe.x = cell.min.x + S + eps;  break;
        case 2: probe.y = cell.min.y - eps;      break;
        case 3: probe.y = cell.min.y + S + eps;  break;
        case 4: probe.z = cell.min.z - eps;      break;
        case 5: probe.z = cell.min.z + S + eps;  break;
      }
      int nb = findLeaf(cells, probe);
      bool hasFinerNeighbour = (nb >= 0 &&
                                cells[nb].size < S - 1e-6f);

      if (!hasFinerNeighbour) {
        // ------- Standard 2-tet face (same-size / coarser / boundary)
        unsigned int c00 = faceCorner(f, 0, 0);
        unsigned int c10 = faceCorner(f, 1, 0);
        unsigned int c01 = faceCorner(f, 0, 1);
        unsigned int c11 = faceCorner(f, 1, 1);

        if (cellParity == 0) {
          out.tets.push_back({c00, c10, c11, bc});
          out.tets.push_back({c00, c11, c01, bc});
        } else {
          out.tets.push_back({c00, c10, c01, bc});
          out.tets.push_back({c10, c11, c01, bc});
        }
      } else {
        // ------- Transition 8-tet face (finer neighbour) ------
        // Build 3×3 grid of vertices on this face
        unsigned int fg[3][3]; // [u_idx][v_idx]  (0=min, 1=mid, 2=max)
        fg[0][0] = faceCorner(f, 0, 0);
        fg[2][0] = faceCorner(f, 1, 0);
        fg[0][2] = faceCorner(f, 0, 1);
        fg[2][2] = faceCorner(f, 1, 1);

        // Build positions for the 5 midpoint / centre vertices
        float base[3] = {cell.min.x, cell.min.y, cell.min.z};
        auto makeFacePos = [&](float uOff, float vOff) -> Vec3f {
          float pos[3] = {base[0], base[1], base[2]};
          pos[fi.axis]  += (fi.dir == 1) ? S : 0.0f;
          pos[fi.uAxis] += uOff;
          pos[fi.vAxis] += vOff;
          return Vec3f(pos[0], pos[1], pos[2]);
        };

        fg[1][0] = getVertex(makeFacePos(hs, 0.0f), false);
        fg[1][2] = getVertex(makeFacePos(hs, S),     false);
        fg[0][1] = getVertex(makeFacePos(0.0f, hs),  false);
        fg[2][1] = getVertex(makeFacePos(S, hs),     false);
        fg[1][1] = getVertex(makeFacePos(hs, hs),   false);

        // Sub-face parity offset: negative faces add 1
        int parityOff = (fi.dir == 0) ? 1 : 0;

        for (int sv = 0; sv < 2; ++sv) {
          for (int su = 0; su < 2; ++su) {
            int sp = ((su + sv + parityOff) % 2 + 2) % 2;

            unsigned int A = fg[su]    [sv];
            unsigned int B = fg[su + 1][sv];
            unsigned int C = fg[su]    [sv + 1];
            unsigned int D = fg[su + 1][sv + 1];

            if (sp == 0) {
              // Diagonal A–D
              out.tets.push_back({A, B, D, bc});
              out.tets.push_back({A, D, C, bc});
            } else {
              // Diagonal B–C
              out.tets.push_back({A, B, C, bc});
              out.tets.push_back({B, D, C, bc});
            }
          }
        }
      }
    } // faces
  } // leaves

  // ---------- 5. Evaluate SDF at every vertex --------------------------
  for (size_t i = 0; i < out.vertices.size(); ++i) {
    out.sdfValues[i] = sdf.evaluate(out.vertices[i]);
  }

  std::cout << "GradedBCC: " << out.vertices.size() << " vertices, "
            << out.tets.size() << " tetrahedra\n";
}
