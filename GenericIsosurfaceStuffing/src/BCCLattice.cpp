/*
 * BCCLattice.cpp
 */

#include "BCCLattice.h"

#include "SignedDistanceField.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace {

/// An axis-aligned cell: minimum corner and edge length, both in units of the
/// finest cell size.
struct Leaf {
  int x, y, z, s;
};

int ilog2(int s) {
  int r = 0;
  while ((1 << r) < s) ++r;
  return r;
}

std::uint64_t cellKey(int x, int y, int z, int lvl) {
  return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(x)) & 0x1FFFFFull) |
         ((static_cast<std::uint64_t>(static_cast<std::uint32_t>(y)) & 0x1FFFFFull) << 21) |
         ((static_cast<std::uint64_t>(static_cast<std::uint32_t>(z)) & 0x1FFFFFull) << 42) |
         (static_cast<std::uint64_t>(static_cast<std::uint32_t>(lvl)) << 58);
}

std::uint64_t vertexKey(int x, int y, int z) {
  return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(x)) & 0x1FFFFFull) |
         ((static_cast<std::uint64_t>(static_cast<std::uint32_t>(y)) & 0x1FFFFFull) << 21) |
         ((static_cast<std::uint64_t>(static_cast<std::uint32_t>(z)) & 0x1FFFFFull) << 42);
}

/// Coarsest cell edge at the lowest grading, in units of the finest cell size.
const int kMinCoarseCell = 8;

}  // namespace

void BCCLattice::build(const Vec3d &bbMin, const Vec3d &bbMax, double h,
                       double grading, const SignedDistanceField &sdf) {
  positions_.clear();
  isCenter_.clear();
  tets_.clear();
  cellCount_ = 0;
  if (h <= 0.0) return;
  const bool graded = grading > 0.0;
  const double g = std::min(std::max(grading, 0.0), 1.0);
  // Interior cells may grow up to 8h .. 128h.
  const int maxCoarseCell = kMinCoarseCell << static_cast<int>(std::lround(g * 4.0));
  // Cells cut by the surface may stay coarse where the surface is flat.
  const int maxSurfaceCell = g < 0.5 ? 2 : 4;
  // Allowed deviation of the distance field from linear inside such a cell.
  const double flatTol = 0.3 * g * h;
  // A cell farther than half its diagonal (0.866 s h) cannot meet the surface.
  const double nearFactor = 1.0 - 0.1 * g;

  // Pad so that the outermost cells are safely outside the surface; the
  // pyramids over domain-boundary faces are not generated.
  const double pad = 2.0 * h;
  const Vec3d lo = bbMin - Vec3d{pad, pad, pad};
  const Vec3d size = (bbMax - bbMin) + Vec3d{2.0 * pad, 2.0 * pad, 2.0 * pad};

  int nx = std::max(3, static_cast<int>(std::ceil(size.x / h)) + 1);
  int ny = std::max(3, static_cast<int>(std::ceil(size.y / h)) + 1);
  int nz = std::max(3, static_cast<int>(std::ceil(size.z / h)) + 1);

  Vec3d origin = lo;
  int extent[3] = {nx, ny, nz};
  int maxCell = 1;

  std::vector<Leaf> leaves;
  std::vector<unsigned char> alive;
  std::unordered_map<std::uint64_t, int> cellMap;

  auto addLeaf = [&](int x, int y, int z, int s) {
    const int idx = static_cast<int>(leaves.size());
    leaves.push_back(Leaf{x, y, z, s});
    alive.push_back(1);
    cellMap[cellKey(x, y, z, ilog2(s))] = idx;
  };

  if (!graded) {
    leaves.reserve(static_cast<std::size_t>(nx) * ny * nz);
    for (int k = 0; k < nz; ++k)
      for (int j = 0; j < ny; ++j)
        for (int i = 0; i < nx; ++i) addLeaf(i, j, k, 1);
  } else {
    int root = 1;
    while (root < std::max(nx, std::max(ny, nz))) root <<= 1;
    extent[0] = extent[1] = extent[2] = root;
    maxCell = root;
    // Centre the padded box inside the cube.
    origin = {lo.x - (root * h - size.x) * 0.5, lo.y - (root * h - size.y) * 0.5,
              lo.z - (root * h - size.z) * 0.5};

    struct Pending {
      int x, y, z, s;
    };
    auto cellPoint = [&](const Pending &c, int i, int j, int k) {
      return origin + Vec3d{(c.x + c.s * 0.5 * i) * h, (c.y + c.s * 0.5 * j) * h,
                            (c.z + c.s * 0.5 * k) * h};
    };
    // True if the distance field is close to trilinear over the cell, i.e. the
    // surface inside is nearly planar and has no edge, corner or thin part.
    auto nearlyFlat = [&](const Pending &c) {
      double f[3][3][3];
      for (int k = 0; k < 3; ++k)
        for (int j = 0; j < 3; ++j)
          for (int i = 0; i < 3; ++i)
            f[i][j][k] = sdf.signedDistance(cellPoint(c, i, j, k));
      for (int k = 0; k < 3; ++k) {
        for (int j = 0; j < 3; ++j) {
          for (int i = 0; i < 3; ++i) {
            if (!((i | j | k) & 1)) continue;  // corners interpolate exactly
            double interp = 0.0;
            for (int o = 0; o < 8; ++o) {
              const int a = o & 1, b = (o >> 1) & 1, d = (o >> 2) & 1;
              const double w = (a ? i : 2 - i) * (b ? j : 2 - j) * (d ? k : 2 - k) / 8.0;
              interp += w * f[2 * a][2 * b][2 * d];
            }
            if (std::fabs(interp - f[i][j][k]) > flatTol) return false;
          }
        }
      }
      return true;
    };
    // The refinement decisions (distance field queries) are made level by level
    // in parallel; the leaves are then collected depth first, in the same order
    // as a sequential traversal, so the result does not depend on threading.
    auto refineCell = [&](const Pending &c) {
      if (c.s <= 1) return false;
      if (c.s > maxCoarseCell) return true;
      const double d = std::fabs(sdf.signedDistance(cellPoint(c, 1, 1, 1)));
      if (d >= nearFactor * c.s * h) return false;
      if (c.s <= maxSurfaceCell && flatTol > 0.0) return !nearlyFlat(c);
      return true;
    };
    std::vector<std::vector<Pending>> levels{{Pending{0, 0, 0, root}}};
    std::vector<std::vector<int>> firstChild;  // per level, -1 for a leaf
    while (!levels.back().empty()) {
      const std::vector<Pending> &cells = levels.back();
      const int n = static_cast<int>(cells.size());
      std::vector<unsigned char> refine(n, 0);
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 64)
#endif
      for (int i = 0; i < n; ++i) refine[i] = refineCell(cells[i]) ? 1 : 0;
      std::vector<int> first(n, -1);
      std::vector<Pending> next;
      for (int i = 0; i < n; ++i) {
        if (!refine[i]) continue;
        first[i] = static_cast<int>(next.size());
        const Pending &c = cells[i];
        const int hs = c.s / 2;
        for (int o = 0; o < 8; ++o)
          next.push_back(Pending{c.x + ((o & 1) ? hs : 0), c.y + ((o & 2) ? hs : 0),
                                 c.z + ((o & 4) ? hs : 0), hs});
      }
      firstChild.push_back(std::move(first));
      levels.push_back(std::move(next));
    }
    std::vector<std::pair<int, int>> stack{{0, 0}};  // level, index
    while (!stack.empty()) {
      const auto [lvl, idx] = stack.back();
      stack.pop_back();
      const Pending &c = levels[lvl][idx];
      const int fc = firstChild[lvl][idx];
      if (fc < 0) {
        addLeaf(c.x, c.y, c.z, c.s);
        continue;
      }
      for (int o = 0; o < 8; ++o) stack.push_back({lvl + 1, fc + o});
    }
  }

  auto leafAt = [&](int x, int y, int z) -> int {
    if (x < 0 || y < 0 || z < 0 || x >= extent[0] || y >= extent[1] || z >= extent[2])
      return -1;
    for (int s = 1; s <= maxCell; s <<= 1) {
      const int m = ~(s - 1);
      const auto it = cellMap.find(cellKey(x & m, y & m, z & m, ilog2(s)));
      if (it != cellMap.end()) return it->second;
    }
    return -1;
  };

  auto probe = [](const Leaf &L, int dir, int &px, int &py, int &pz) {
    px = L.x;
    py = L.y;
    pz = L.z;
    switch (dir) {
      case 0: px = L.x + L.s; break;
      case 1: px = L.x - 1; break;
      case 2: py = L.y + L.s; break;
      case 3: py = L.y - 1; break;
      case 4: pz = L.z + L.s; break;
      default: pz = L.z - 1; break;
    }
  };

  auto splitLeaf = [&](int idx) {
    const Leaf N = leaves[idx];
    alive[idx] = 0;
    cellMap.erase(cellKey(N.x, N.y, N.z, ilog2(N.s)));
    const int hs = N.s / 2;
    for (int o = 0; o < 8; ++o) {
      addLeaf(N.x + ((o & 1) ? hs : 0), N.y + ((o & 2) ? hs : 0),
              N.z + ((o & 4) ? hs : 0), hs);
    }
  };

  if (graded) {
    // Strong 2:1 balance: cells meeting at a face, an edge or a corner differ
    // by at most one level. That bounds a cell edge to a single split point,
    // which is what keeps the face patches below conforming.
    // One pass suffices: split cells append their children to the list, and
    // those check (and split) their own neighbours when the pass reaches them.
    for (std::size_t li = 0; li < leaves.size(); ++li) {
      if (!alive[li]) continue;
      const Leaf F = leaves[li];
      for (int dz = -1; dz <= 1; ++dz) {
        for (int dy = -1; dy <= 1; ++dy) {
          for (int dx = -1; dx <= 1; ++dx) {
            if (!dx && !dy && !dz) continue;
            const int px = F.x + (dx < 0 ? -1 : (dx > 0 ? F.s : 0));
            const int py = F.y + (dy < 0 ? -1 : (dy > 0 ? F.s : 0));
            const int pz = F.z + (dz < 0 ? -1 : (dz > 0 ? F.s : 0));
            const int ni = leafAt(px, py, pz);
            if (ni < 0 || !alive[ni]) continue;
            if (leaves[ni].s <= 2 * F.s) continue;
            splitLeaf(ni);
          }
        }
      }
    }
  }

  // Every corner of every cell, in half-cell coordinates. A cell edge is split
  // exactly when its midpoint is a corner of some other cell.
  std::unordered_set<std::uint64_t> cornerSet;
  for (std::size_t li = 0; li < leaves.size(); ++li) {
    if (!alive[li]) continue;
    const Leaf L = leaves[li];
    for (int o = 0; o < 8; ++o) {
      cornerSet.insert(vertexKey(2 * (L.x + ((o & 1) ? L.s : 0)),
                                 2 * (L.y + ((o & 2) ? L.s : 0)),
                                 2 * (L.z + ((o & 4) ? L.s : 0))));
    }
  }

  // Lattice points live on a grid of half-cells, so corners have even and
  // centres odd coordinates within a cell of size 1.
  std::unordered_map<std::uint64_t, int> vertexMap;
  auto vertexAt = [&](int X, int Y, int Z, bool centre) {
    const std::uint64_t k = vertexKey(X, Y, Z);
    const auto it = vertexMap.find(k);
    if (it != vertexMap.end()) return it->second;
    const int idx = static_cast<int>(positions_.size());
    positions_.push_back(origin + Vec3d{X * 0.5 * h, Y * 0.5 * h, Z * 0.5 * h});
    isCenter_.push_back(centre ? 1 : 0);
    vertexMap.emplace(k, idx);
    return idx;
  };

  for (std::size_t li = 0; li < leaves.size(); ++li) {
    if (!alive[li]) continue;
    ++cellCount_;
    const Leaf L = leaves[li];
    for (int d = 0; d < 6; ++d) {
      int px, py, pz;
      probe(L, d, px, py, pz);
      const int ni = leafAt(px, py, pz);
      if (ni < 0) continue;  // domain boundary: nothing to fill
      const Leaf N = leaves[ni];
      if (N.s < L.s) continue;              // the finer side emits this face
      if (N.s == L.s && (d & 1)) continue;  // emit equal-size faces once

      const int s = L.s;
      const int x0 = L.x, x1 = L.x + s;
      const int y0 = L.y, y1 = L.y + s;
      const int z0 = L.z, z1 = L.z + s;
      int c[4][3];
      switch (d) {
        case 0:
        case 1: {
          const int X = (d == 0) ? x1 : x0;
          c[0][0] = X; c[0][1] = y0; c[0][2] = z0;
          c[1][0] = X; c[1][1] = y1; c[1][2] = z0;
          c[2][0] = X; c[2][1] = y1; c[2][2] = z1;
          c[3][0] = X; c[3][1] = y0; c[3][2] = z1;
          break;
        }
        case 2:
        case 3: {
          const int Y = (d == 2) ? y1 : y0;
          c[0][0] = x0; c[0][1] = Y; c[0][2] = z0;
          c[1][0] = x1; c[1][1] = Y; c[1][2] = z0;
          c[2][0] = x1; c[2][1] = Y; c[2][2] = z1;
          c[3][0] = x0; c[3][1] = Y; c[3][2] = z1;
          break;
        }
        default: {
          const int Z = (d == 4) ? z1 : z0;
          c[0][0] = x0; c[0][1] = y0; c[0][2] = Z;
          c[1][0] = x1; c[1][1] = y0; c[1][2] = Z;
          c[2][0] = x1; c[2][1] = y1; c[2][2] = Z;
          c[3][0] = x0; c[3][1] = y1; c[3][2] = Z;
          break;
        }
      }

      const int cl = vertexAt(2 * L.x + L.s, 2 * L.y + L.s, 2 * L.z + L.s, true);
      const int cn = vertexAt(2 * N.x + N.s, 2 * N.y + N.s, 2 * N.z + N.s, true);

      // Walk the face boundary, inserting the midpoint of every split edge so
      // that neighbouring patches agree along their shared edge.
      int poly[8][3];
      int np = 0;
      for (int i = 0; i < 4; ++i) {
        const int j = (i + 1) % 4;
        poly[np][0] = 2 * c[i][0];
        poly[np][1] = 2 * c[i][1];
        poly[np][2] = 2 * c[i][2];
        ++np;
        const int m0 = c[i][0] + c[j][0];
        const int m1 = c[i][1] + c[j][1];
        const int m2 = c[i][2] + c[j][2];
        if (cornerSet.count(vertexKey(m0, m1, m2)) != 0) {
          poly[np][0] = m0;
          poly[np][1] = m1;
          poly[np][2] = m2;
          ++np;
        }
      }

      int pid[8];
      for (int i = 0; i < np; ++i)
        pid[i] = vertexAt(poly[i][0], poly[i][1], poly[i][2], false);

      if (np == 4) {
        // Plain square face: the BCC bipyramid around the cl-cn axis.
        for (int i = 0; i < 4; ++i)
          tets_.push_back(BccTet{{cl, cn, pid[i], pid[(i + 1) % 4]}});
      } else {
        const int fc = vertexAt(c[0][0] + c[2][0], c[0][1] + c[2][1],
                                c[0][2] + c[2][2], false);
        for (int i = 0; i < np; ++i) {
          const int a = pid[i], b = pid[(i + 1) % np];
          tets_.push_back(BccTet{{cl, fc, a, b}});
          tets_.push_back(BccTet{{cn, fc, a, b}});
        }
      }
    }
  }
}
