/*
 * SurfaceChecker.cpp
 */

#include <TetraMeshTools/SurfaceChecker.h>
#include <TetraMeshTools/Vec3d.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <map>
#include <tuple>

namespace {

typedef std::array<Vec3d, 3> Tri;

/// Closed segment-triangle test (non-parallel case only).
bool segmentHitsTriangle(const Vec3d &p, const Vec3d &q, const Tri &t) {
  const double eps = 1e-9;
  const Vec3d dir = q - p;
  const Vec3d e1 = t[1] - t[0];
  const Vec3d e2 = t[2] - t[0];
  const Vec3d h = cross(dir, e2);
  const double det = dot(e1, h);
  if (std::fabs(det) <= 1e-14 * length(dir) * length(e1) * length(e2)) return false;
  const double f = 1.0 / det;
  const Vec3d s = p - t[0];
  const double u = f * dot(s, h);
  if (u < -eps || u > 1.0 + eps) return false;
  const Vec3d qv = cross(s, e1);
  const double v = f * dot(dir, qv);
  if (v < -eps || u + v > 1.0 + eps) return false;
  const double tt = f * dot(e2, qv);
  return tt >= -eps && tt <= 1.0 + eps;
}

struct P2 {
  double x, y;
};

double orient2(const P2 &a, const P2 &b, const P2 &c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool properCross(const P2 &a, const P2 &b, const P2 &c, const P2 &d) {
  return orient2(a, b, c) * orient2(a, b, d) < 0 &&
         orient2(c, d, a) * orient2(c, d, b) < 0;
}

bool inTriangle(const P2 &p, const std::array<P2, 3> &t, bool strict) {
  const double o0 = orient2(t[0], t[1], p);
  const double o1 = orient2(t[1], t[2], p);
  const double o2 = orient2(t[2], t[0], p);
  if (strict) return (o0 > 0 && o1 > 0 && o2 > 0) || (o0 < 0 && o1 < 0 && o2 < 0);
  return (o0 >= 0 && o1 >= 0 && o2 >= 0) || (o0 <= 0 && o1 <= 0 && o2 <= 0);
}

/// Overlap of two coplanar triangles; `shared` touching is not an overlap.
bool coplanarOverlap(const Tri &a, const Tri &b, const Vec3d &normal, int shared) {
  // Drop the dominant normal axis.
  const double ax = std::fabs(normal.x), ay = std::fabs(normal.y),
               az = std::fabs(normal.z);
  auto proj = [&](const Vec3d &v) -> P2 {
    if (ax >= ay && ax >= az) return {v.y, v.z};
    if (ay >= az) return {v.x, v.z};
    return {v.x, v.y};
  };
  std::array<P2, 3> pa, pb;
  for (int i = 0; i < 3; ++i) {
    pa[i] = proj(a[i]);
    pb[i] = proj(b[i]);
  }
  const bool strict = shared > 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (properCross(pa[i], pa[(i + 1) % 3], pb[j], pb[(j + 1) % 3])) return true;
    }
  }
  for (int i = 0; i < 3; ++i) {
    if (inTriangle(pa[i], pb, strict) || inTriangle(pb[i], pa, strict)) return true;
  }
  const P2 ca = {(pa[0].x + pa[1].x + pa[2].x) / 3.0, (pa[0].y + pa[1].y + pa[2].y) / 3.0};
  const P2 cb = {(pb[0].x + pb[1].x + pb[2].x) / 3.0, (pb[0].y + pb[1].y + pb[2].y) / 3.0};
  return inTriangle(ca, pb, true) || inTriangle(cb, pa, true);
}

}  // namespace

SurfaceCheckResult CheckSurfaceMesh(const std::vector<Vec3f> &verts,
                                    const std::vector<Triangle> &tris,
                                    const std::atomic<bool> *cancel) {
  SurfaceCheckResult res;
  res.numTriangles = tris.size();
  if (verts.empty() || tris.empty()) return res;

  // Weld vertices with identical coordinates so unwelded meshes are judged by
  // geometry rather than by their index layout.
  std::vector<std::uint32_t> canon(verts.size());
  {
    std::map<std::tuple<float, float, float>, std::uint32_t> seen;
    for (std::size_t i = 0; i < verts.size(); ++i) {
      auto key = std::make_tuple(verts[i].x, verts[i].y, verts[i].z);
      auto it = seen.find(key);
      if (it == seen.end()) it = seen.emplace(key, static_cast<std::uint32_t>(i)).first;
      canon[i] = it->second;
    }
  }

  std::vector<Vec3d> pts(verts.size());
  Vec3d bbMin = {1e300, 1e300, 1e300}, bbMax = {-1e300, -1e300, -1e300};
  for (std::size_t i = 0; i < verts.size(); ++i) {
    pts[i] = {verts[i].x, verts[i].y, verts[i].z};
    bbMin = {std::min(bbMin.x, pts[i].x), std::min(bbMin.y, pts[i].y), std::min(bbMin.z, pts[i].z)};
    bbMax = {std::max(bbMax.x, pts[i].x), std::max(bbMax.y, pts[i].y), std::max(bbMax.z, pts[i].z)};
  }
  const Vec3d ext = bbMax - bbMin;
  const double diag = length(ext);
  const double tol = 1e-9 * diag;

  const std::size_t n = tris.size();
  std::vector<std::array<std::uint32_t, 3>> ci(n);
  std::vector<Tri> tv(n);
  std::vector<Vec3d> nrm(n);
  std::vector<char> degenerate(n, 0);
  for (std::size_t i = 0; i < n; ++i) {
    for (int k = 0; k < 3; ++k) {
      ci[i][k] = canon[tris[i].index[k]];
      tv[i][k] = pts[tris[i].index[k]];
    }
    nrm[i] = cross(tv[i][1] - tv[i][0], tv[i][2] - tv[i][0]);
    if (ci[i][0] == ci[i][1] || ci[i][1] == ci[i][2] || ci[i][0] == ci[i][2] ||
        length(nrm[i]) <= 1e-12 * diag * diag) {
      degenerate[i] = 1;
      ++res.numDegenerateTriangles;
    }
  }

  // Edge topology: open, non-manifold and inconsistently oriented edges,
  // plus component count and genus.
  {
    std::vector<std::uint64_t> keys, directed;
    keys.reserve(n * 3);
    directed.reserve(n * 3);
    std::vector<std::uint32_t> parent(verts.size());
    for (std::size_t i = 0; i < parent.size(); ++i) parent[i] = static_cast<std::uint32_t>(i);
    auto find = [&](std::uint32_t x) {
      while (parent[x] != x) x = parent[x] = parent[parent[x]];
      return x;
    };
    std::vector<char> used(verts.size(), 0);
    for (std::size_t i = 0; i < n; ++i) {
      for (int k = 0; k < 3; ++k) {
        std::uint64_t a = ci[i][k], b = ci[i][(k + 1) % 3];
        used[a] = 1;
        parent[find(static_cast<std::uint32_t>(a))] = find(static_cast<std::uint32_t>(b));
        if (a == b) continue;
        directed.push_back((a << 32) | b);
        if (a > b) std::swap(a, b);
        keys.push_back((a << 32) | b);
      }
    }
    std::sort(keys.begin(), keys.end());
    std::sort(directed.begin(), directed.end());
    std::size_t numEdges = 0;
    for (std::size_t i = 0; i < keys.size();) {
      std::size_t j = i;
      while (j < keys.size() && keys[j] == keys[i]) ++j;
      ++numEdges;
      if (j - i == 1) ++res.numBoundaryEdges;
      else if (j - i > 2) ++res.numNonManifoldEdges;
      i = j;
    }
    for (std::size_t i = 1; i < directed.size(); ++i)
      if (directed[i] == directed[i - 1]) ++res.numInconsistentEdges;

    std::size_t numUsedCanonical = 0;
    std::vector<char> root(verts.size(), 0);
    for (std::size_t i = 0; i < verts.size(); ++i) {
      if (!used[i]) continue;
      ++numUsedCanonical;
      root[find(static_cast<std::uint32_t>(i))] = 1;
    }
    res.numComponents = static_cast<std::size_t>(std::count(root.begin(), root.end(), 1));
    // Unused = vertices (before welding) not referenced by any triangle.
    std::vector<char> referenced(verts.size(), 0);
    for (const auto &t : tris)
      for (int k = 0; k < 3; ++k) referenced[t.index[k]] = 1;
    res.numUnusedVertices = static_cast<std::size_t>(
        std::count(referenced.begin(), referenced.end(), 0));
    if (res.numBoundaryEdges == 0 && res.numNonManifoldEdges == 0) {
      const long chi = static_cast<long>(numUsedCanonical) - static_cast<long>(numEdges) +
                       static_cast<long>(n);
      res.genus = static_cast<int>((2 * static_cast<long>(res.numComponents) - chi) / 2);
    }
  }

  // Uniform grid broad phase over triangle bounding boxes.
  std::vector<Vec3d> tmin(n), tmax(n);
  double meanExtent = 0.0;
  for (std::size_t i = 0; i < n; ++i) {
    tmin[i] = tmax[i] = tv[i][0];
    for (int k = 1; k < 3; ++k) {
      tmin[i] = {std::min(tmin[i].x, tv[i][k].x), std::min(tmin[i].y, tv[i][k].y), std::min(tmin[i].z, tv[i][k].z)};
      tmax[i] = {std::max(tmax[i].x, tv[i][k].x), std::max(tmax[i].y, tv[i][k].y), std::max(tmax[i].z, tv[i][k].z)};
    }
    tmin[i] = {tmin[i].x - tol, tmin[i].y - tol, tmin[i].z - tol};
    tmax[i] = {tmax[i].x + tol, tmax[i].y + tol, tmax[i].z + tol};
    meanExtent += std::max({tmax[i].x - tmin[i].x, tmax[i].y - tmin[i].y, tmax[i].z - tmin[i].z});
  }
  meanExtent /= static_cast<double>(n);

  const std::size_t maxCells = 2000000;
  double h = std::max(meanExtent, diag / 512.0);
  if (h <= 0.0) return res;
  int dim[3];
  for (;;) {
    dim[0] = std::max(1, static_cast<int>(std::ceil(ext.x / h)));
    dim[1] = std::max(1, static_cast<int>(std::ceil(ext.y / h)));
    dim[2] = std::max(1, static_cast<int>(std::ceil(ext.z / h)));
    if (static_cast<std::size_t>(dim[0]) * dim[1] * dim[2] <= maxCells) break;
    h *= 1.5;
  }
  auto cellOf = [&](double v, double lo, int axisDim) {
    int c = static_cast<int>(std::floor((v - lo) / h));
    return std::min(std::max(c, 0), axisDim - 1);
  };
  const std::size_t numCells = static_cast<std::size_t>(dim[0]) * dim[1] * dim[2];
  auto cellIndex = [&](int cx, int cy, int cz) {
    return (static_cast<std::size_t>(cz) * dim[1] + cy) * dim[0] + cx;
  };

  std::vector<std::uint32_t> cellStart(numCells + 1, 0);
  std::vector<std::uint32_t> entries;
  for (int pass = 0; pass < 2; ++pass) {
    std::vector<std::uint32_t> fill(numCells, 0);
    for (std::size_t i = 0; i < n; ++i) {
      if (degenerate[i]) continue;
      const int x0 = cellOf(tmin[i].x, bbMin.x, dim[0]), x1 = cellOf(tmax[i].x, bbMin.x, dim[0]);
      const int y0 = cellOf(tmin[i].y, bbMin.y, dim[1]), y1 = cellOf(tmax[i].y, bbMin.y, dim[1]);
      const int z0 = cellOf(tmin[i].z, bbMin.z, dim[2]), z1 = cellOf(tmax[i].z, bbMin.z, dim[2]);
      for (int z = z0; z <= z1; ++z)
        for (int y = y0; y <= y1; ++y)
          for (int x = x0; x <= x1; ++x) {
            const std::size_t c = cellIndex(x, y, z);
            if (pass == 0) ++cellStart[c + 1];
            else entries[cellStart[c] + fill[c]++] = static_cast<std::uint32_t>(i);
          }
    }
    if (pass == 0) {
      for (std::size_t c = 0; c < numCells; ++c) cellStart[c + 1] += cellStart[c];
      entries.resize(cellStart[numCells]);
    }
  }

  auto sharedVertices = [&](std::size_t a, std::size_t b, int &sa0, int &sb0) {
    int count = 0;
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
        if (ci[a][i] == ci[b][j]) {
          ++count;
          sa0 = i;
          sb0 = j;
        }
    return count;
  };

  auto planeSide = [&](const Tri &t, const Vec3d &n0, const Tri &plane) {
    // 1: all strictly above, -1: all strictly below, 0 otherwise
    int above = 0, below = 0;
    const double nl = length(n0);
    for (int k = 0; k < 3; ++k) {
      const double d = dot(n0, t[k] - plane[0]) / nl;
      if (d > tol) ++above;
      else if (d < -tol) ++below;
    }
    return above == 3 ? 1 : (below == 3 ? -1 : 0);
  };

  auto trianglesIntersect = [&](std::size_t a, std::size_t b) {
    int sa = -1, sb = -1;
    const int shared = sharedVertices(a, b, sa, sb);
    if (shared == 3) return true;  // duplicate triangle
    if (planeSide(tv[b], nrm[a], tv[a]) != 0) return false;
    if (planeSide(tv[a], nrm[b], tv[b]) != 0) return false;

    // Coplanar if all vertices of b lie in the plane of a.
    bool coplanar = true;
    const double na = length(nrm[a]);
    for (int k = 0; k < 3; ++k)
      if (std::fabs(dot(nrm[a], tv[b][k] - tv[a][0])) / na > tol) coplanar = false;

    if (coplanar) {
      if (shared == 2) {
        // Fold-over: the free vertices lie on the same side of the shared edge.
        int ia = -1, ib = -1;
        std::uint32_t e0 = 0, e1 = 0;
        bool first = true;
        for (int i = 0; i < 3; ++i)
          for (int j = 0; j < 3; ++j)
            if (ci[a][i] == ci[b][j]) {
              if (first) { e0 = ci[a][i]; first = false; }
              else e1 = ci[a][i];
            }
        for (int i = 0; i < 3; ++i) if (ci[a][i] != e0 && ci[a][i] != e1) ia = i;
        for (int j = 0; j < 3; ++j) if (ci[b][j] != e0 && ci[b][j] != e1) ib = j;
        const Vec3d &p0 = pts[e0], &p1 = pts[e1];
        const double sA = dot(cross(p1 - p0, tv[a][ia] - p0), nrm[a]);
        const double sB = dot(cross(p1 - p0, tv[b][ib] - p0), nrm[a]);
        return sA * sB > 0;
      }
      return coplanarOverlap(tv[a], tv[b], nrm[a], shared);
    }

    if (shared == 2) return false;  // meet along the shared edge only
    // Edges not containing the shared vertex (all edges when shared == 0).
    auto edgesHit = [&](std::size_t x, std::size_t y, int sharedIdx) {
      for (int k = 0; k < 3; ++k) {
        const int k2 = (k + 1) % 3;
        if (shared == 1 && (k == sharedIdx || k2 == sharedIdx)) continue;
        if (segmentHitsTriangle(tv[x][k], tv[x][k2], tv[y])) return true;
      }
      return false;
    };
    return edgesHit(a, b, sa) || edgesHit(b, a, sb);
  };

  std::vector<char> inPair(n, 0);
  for (std::size_t c = 0; c < numCells; ++c) {
    // the pair tests are the expensive part
    if (cancel && cancel->load(std::memory_order_relaxed)) {
      res.canceled = true;
      return res;
    }
    const std::uint32_t begin = cellStart[c], end = cellStart[c + 1];
    if (end - begin < 2) continue;
    const std::size_t cx = c % dim[0];
    const std::size_t cy = (c / dim[0]) % dim[1];
    const std::size_t cz = c / (static_cast<std::size_t>(dim[0]) * dim[1]);
    for (std::uint32_t i = begin; i < end; ++i) {
      for (std::uint32_t j = i + 1; j < end; ++j) {
        const std::size_t a = entries[i], b = entries[j];
        const double ox = std::max(tmin[a].x, tmin[b].x), oy = std::max(tmin[a].y, tmin[b].y),
                     oz = std::max(tmin[a].z, tmin[b].z);
        if (ox > std::min(tmax[a].x, tmax[b].x) || oy > std::min(tmax[a].y, tmax[b].y) ||
            oz > std::min(tmax[a].z, tmax[b].z))
          continue;
        // Test each pair only in the cell containing its overlap's min corner.
        if (static_cast<std::size_t>(cellOf(ox, bbMin.x, dim[0])) != cx ||
            static_cast<std::size_t>(cellOf(oy, bbMin.y, dim[1])) != cy ||
            static_cast<std::size_t>(cellOf(oz, bbMin.z, dim[2])) != cz)
          continue;
        if (trianglesIntersect(a, b)) {
          ++res.numIntersectingPairs;
          inPair[a] = inPair[b] = 1;
          if (res.examplePairs.size() < 20)
            res.examplePairs.emplace_back(static_cast<unsigned int>(a), static_cast<unsigned int>(b));
        }
      }
    }
  }
  res.numIntersectingTriangles = static_cast<std::size_t>(std::count(inPair.begin(), inPair.end(), 1));
  return res;
}
