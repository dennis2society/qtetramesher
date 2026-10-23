/*
 * MeshOptimizer.cpp
 */

#include "MeshOptimizer.h"

#include "SignedDistanceField.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <set>
#include <unordered_map>
#include <utility>

namespace {

/// Snapping may not push a tetrahedron below this (unless it already was).
const double kQualityFloor = 0.1;
/// Vertices of tetrahedra below this quality are smoothed.
const double kSmoothTarget = 0.3;
const int kSmoothPasses = 4;
const int kSnapRounds = 3;
/// Flat tetrahedra on the surface below this quality are removed.
const double kCapQuality = 0.1;
/// Tetrahedra created to fill notches at sharp features must reach this.
const double kInsertQuality = 0.05;

Vec3d closestOnTriangle(const Vec3d &p, const Vec3d &a, const Vec3d &b, const Vec3d &c) {
  const Vec3d ab = b - a, ac = c - a, ap = p - a;
  const double d1 = dot(ab, ap), d2 = dot(ac, ap);
  if (d1 <= 0.0 && d2 <= 0.0) return a;
  const Vec3d bp = p - b;
  const double d3 = dot(ab, bp), d4 = dot(ac, bp);
  if (d3 >= 0.0 && d4 <= d3) return b;
  const double vc = d1 * d4 - d3 * d2;
  if (vc <= 0.0 && d1 >= 0.0 && d3 <= 0.0) return a + ab * (d1 / (d1 - d3));
  const Vec3d cp = p - c;
  const double d5 = dot(ab, cp), d6 = dot(ac, cp);
  if (d6 >= 0.0 && d5 <= d6) return c;
  const double vb = d5 * d2 - d1 * d6;
  if (vb <= 0.0 && d2 >= 0.0 && d6 <= 0.0) return a + ac * (d2 / (d2 - d6));
  const double va = d3 * d6 - d5 * d4;
  if (va <= 0.0 && (d4 - d3) >= 0.0 && (d5 - d6) >= 0.0)
    return b + (c - b) * ((d4 - d3) / ((d4 - d3) + (d5 - d6)));
  const double denom = 1.0 / (va + vb + vc);
  return a + ab * (vb * denom) + ac * (vc * denom);
}

/// True if the open segment pq crosses the interior of triangle abc.
bool segmentHitsTriangle(const Vec3d &p, const Vec3d &q, const Vec3d &a, const Vec3d &b,
                         const Vec3d &c) {
  const Vec3d d = q - p, e1 = b - a, e2 = c - a;
  const Vec3d h = cross(d, e2);
  const double det = dot(e1, h);
  if (std::fabs(det) < 1e-14 * dot(e1, e1) * length(d)) return false;
  const double inv = 1.0 / det;
  const Vec3d s = p - a;
  const double u = dot(s, h) * inv;
  if (u <= 1e-9 || u >= 1.0 - 1e-9) return false;
  const Vec3d qv = cross(s, e1);
  const double v = dot(d, qv) * inv;
  if (v <= 1e-9 || u + v >= 1.0 - 1e-9) return false;
  const double t = dot(e2, qv) * inv;
  return t > 1e-9 && t < 1.0 - 1e-9;
}

double tetVolume(const Vec3d &a, const Vec3d &b, const Vec3d &c, const Vec3d &d) {
  return dot(b - a, cross(c - a, d - a)) / 6.0;
}

/// 1 for a regular tetrahedron, 0 for a flat one, negative if inverted.
double tetQuality(const Vec3d &a, const Vec3d &b, const Vec3d &c, const Vec3d &d) {
  const Vec3d e[6] = {b - a, c - a, d - a, c - b, d - b, d - c};
  double s2 = 0.0;
  for (const Vec3d &v : e) s2 += dot(v, v);
  const double lrms = std::sqrt(s2 / 6.0);
  if (!(lrms > 0.0)) return 0.0;
  return 6.0 * std::sqrt(2.0) * tetVolume(a, b, c, d) / (lrms * lrms * lrms);
}

Vec3d closestOnSegment(const Vec3d &p, const Vec3d &a, const Vec3d &b) {
  const Vec3d ab = b - a;
  const double l2 = dot(ab, ab);
  if (!(l2 > 0.0)) return a;
  const double t = std::min(std::max(dot(p - a, ab) / l2, 0.0), 1.0);
  return a + ab * t;
}

/// Uniform hash grid; finds every item within one cell size of a point by
/// visiting the 27 surrounding cells.
class HashGrid {
 public:
  HashGrid(const Vec3d &origin, double cellSize) : origin_(origin), cs_(cellSize) {}

  void insert(const Vec3d &lo, const Vec3d &hi, int id) {
    long x0, y0, z0, x1, y1, z1;
    cellOf(lo, x0, y0, z0);
    cellOf(hi, x1, y1, z1);
    for (long z = z0; z <= z1; ++z)
      for (long y = y0; y <= y1; ++y)
        for (long x = x0; x <= x1; ++x) cells_[key(x, y, z)].push_back(id);
  }

  void insert(const Vec3d &p, int id) { insert(p, p, id); }

  template <typename F>
  void forNear(const Vec3d &p, F f) const {
    long cx, cy, cz;
    cellOf(p, cx, cy, cz);
    for (long z = cz - 1; z <= cz + 1; ++z)
      for (long y = cy - 1; y <= cy + 1; ++y)
        for (long x = cx - 1; x <= cx + 1; ++x) {
          const auto it = cells_.find(key(x, y, z));
          if (it == cells_.end()) continue;
          for (int id : it->second) f(id);
        }
  }

 private:
  void cellOf(const Vec3d &p, long &x, long &y, long &z) const {
    x = static_cast<long>(std::floor((p.x - origin_.x) / cs_));
    y = static_cast<long>(std::floor((p.y - origin_.y) / cs_));
    z = static_cast<long>(std::floor((p.z - origin_.z) / cs_));
  }

  static std::uint64_t key(long x, long y, long z) {
    auto c = [](long v) {
      v += 1L << 20;
      return static_cast<std::uint64_t>(std::min(std::max(v, 0L), 0x1FFFFFL));
    };
    return c(x) | (c(y) << 21) | (c(z) << 42);
  }

  Vec3d origin_;
  double cs_;
  std::unordered_map<std::uint64_t, std::vector<int>> cells_;
};

}  // namespace

MeshOptimizerStats optimizeStuffedMesh(const std::vector<Vec3f> &surfVerts,
                                       const std::vector<Triangle> &surfTris,
                                       const SignedDistanceField &sdf,
                                       double featureAngleDeg, double snapStrength,
                                       std::vector<Vec3d> &pos,
                                       std::vector<std::array<int, 4>> &tets) {
  MeshOptimizerStats st;
  const int nv = static_cast<int>(pos.size());
  if (tets.empty() || surfTris.empty()) return st;
  snapStrength = std::min(std::max(snapStrength, 0.0), 1.0);

  // --- boundary of the tetrahedral mesh ---
  struct Face {
    int v[3];
    bool operator<(const Face &o) const {
      return std::lexicographical_compare(v, v + 3, o.v, o.v + 3);
    }
    bool operator==(const Face &o) const {
      return v[0] == o.v[0] && v[1] == o.v[1] && v[2] == o.v[2];
    }
  };
  std::vector<unsigned char> boundary(nv, 0);
  std::vector<std::vector<int>> boundaryNbrs(nv);
  {
    std::vector<Face> faces;
    faces.reserve(tets.size() * 4);
    for (const auto &t : tets) {
      for (int k = 0; k < 4; ++k) {
        Face f;
        int m = 0;
        for (int c = 0; c < 4; ++c)
          if (c != k) f.v[m++] = t[c];
        std::sort(f.v, f.v + 3);
        faces.push_back(f);
      }
    }
    std::sort(faces.begin(), faces.end());
    for (std::size_t i = 0; i < faces.size();) {
      std::size_t j = i + 1;
      while (j < faces.size() && faces[j] == faces[i]) ++j;
      if (j - i == 1) {
        const Face &f = faces[i];
        for (int k = 0; k < 3; ++k) {
          const int a = f.v[k], b = f.v[(k + 1) % 3];
          boundary[a] = boundary[b] = 1;
          boundaryNbrs[a].push_back(b);
          boundaryNbrs[b].push_back(a);
        }
      }
      i = j;
    }
  }
  std::vector<double> meanLen(nv, 0.0);
  double maxMeanLen = 0.0;
  for (int v = 0; v < nv; ++v) {
    auto &n = boundaryNbrs[v];
    if (n.empty()) continue;
    std::sort(n.begin(), n.end());
    n.erase(std::unique(n.begin(), n.end()), n.end());
    double sum = 0.0;
    for (int u : n) sum += length(pos[u] - pos[v]);
    meanLen[v] = sum / n.size();
    maxMeanLen = std::max(maxMeanLen, meanLen[v]);
  }
  if (!(maxMeanLen > 0.0)) return st;

  // vertex -> tetrahedra
  std::vector<int> tetStart(nv + 1, 0), tetList(tets.size() * 4);
  for (const auto &t : tets)
    for (int k = 0; k < 4; ++k) ++tetStart[t[k] + 1];
  for (int v = 0; v < nv; ++v) tetStart[v + 1] += tetStart[v];
  {
    std::vector<int> fill(tetStart.begin(), tetStart.end() - 1);
    for (std::size_t ti = 0; ti < tets.size(); ++ti)
      for (int k = 0; k < 4; ++k) tetList[fill[tets[ti][k]]++] = static_cast<int>(ti);
  }

  auto quality = [&](int ti) {
    const auto &t = tets[ti];
    return tetQuality(pos[t[0]], pos[t[1]], pos[t[2]], pos[t[3]]);
  };
  auto minQuality = [&]() {
    double q = std::numeric_limits<double>::max();
    const int n = static_cast<int>(tets.size());
#ifdef _OPENMP
#pragma omp parallel for schedule(static) reduction(min : q)
#endif
    for (int ti = 0; ti < n; ++ti) q = std::min(q, quality(ti));
    return q;
  };
  // the tets below a quality, in index order
  auto tetsBelow = [&](double threshold) {
    const int n = static_cast<int>(tets.size());
    std::vector<unsigned char> below(n, 0);
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
    for (int ti = 0; ti < n; ++ti) below[ti] = quality(ti) < threshold;
    std::vector<int> result;
    for (int ti = 0; ti < n; ++ti)
      if (below[ti]) result.push_back(ti);
    return result;
  };
  auto minQualityAround = [&](int v, const Vec3d &p) {
    double qMin = std::numeric_limits<double>::max();
    for (int i = tetStart[v]; i < tetStart[v + 1]; ++i) {
      const auto &t = tets[tetList[i]];
      Vec3d q[4];
      for (int k = 0; k < 4; ++k) q[k] = (t[k] == v) ? p : pos[t[k]];
      qMin = std::min(qMin, tetQuality(q[0], q[1], q[2], q[3]));
    }
    return qMin;
  };
  st.minQualityBefore = minQuality();

  // Snapped vertices stay where they are; vertices near a sharp feature that
  // could not be snapped are not slid along the surface (that could move them
  // across the feature).
  std::vector<unsigned char> fixed(nv, 0);

  // --- sharp edges and corners of the input surface ---
  std::vector<Vec3d> sv(surfVerts.size());
  for (std::size_t i = 0; i < surfVerts.size(); ++i)
    sv[i] = {surfVerts[i].x, surfVerts[i].y, surfVerts[i].z};
  std::vector<std::pair<int, int>> segs;
  std::vector<int> corners;
  if (featureAngleDeg > 0.0) {
    const double cosThr = std::cos(featureAngleDeg * 3.14159265358979323846 / 180.0);
    std::vector<Vec3d> faceNormal(surfTris.size());
    struct EdgeRec {
      int a, b, tri;
      bool forward;  // the triangle runs from a to b
    };
    std::vector<EdgeRec> edgeRecs;
    edgeRecs.reserve(surfTris.size() * 3);
    for (std::size_t t = 0; t < surfTris.size(); ++t) {
      const unsigned int *idx = surfTris[t].index;
      faceNormal[t] = normalized(cross(sv[idx[1]] - sv[idx[0]], sv[idx[2]] - sv[idx[0]]));
      for (int k = 0; k < 3; ++k) {
        const int u = static_cast<int>(idx[k]), w = static_cast<int>(idx[(k + 1) % 3]);
        edgeRecs.push_back({std::min(u, w), std::max(u, w), static_cast<int>(t), u < w});
      }
    }
    std::sort(edgeRecs.begin(), edgeRecs.end(), [](const EdgeRec &x, const EdgeRec &y) {
      return x.a != y.a ? x.a < y.a : x.b < y.b;
    });
    std::vector<std::vector<int>> featureNbrs(sv.size());
    for (std::size_t i = 0; i < edgeRecs.size();) {
      std::size_t j = i + 1;
      while (j < edgeRecs.size() && edgeRecs[j].a == edgeRecs[i].a &&
             edgeRecs[j].b == edgeRecs[i].b)
        ++j;
      bool sharp = (j - i != 2);  // open or non-manifold
      if (!sharp) {
        double c = dot(faceNormal[edgeRecs[i].tri], faceNormal[edgeRecs[i + 1].tri]);
        // consistently oriented neighbours run along the edge in opposite ways
        if (edgeRecs[i].forward == edgeRecs[i + 1].forward) c = -c;
        sharp = c < cosThr;
      }
      if (sharp) {
        segs.push_back({edgeRecs[i].a, edgeRecs[i].b});
        featureNbrs[edgeRecs[i].a].push_back(edgeRecs[i].b);
        featureNbrs[edgeRecs[i].b].push_back(edgeRecs[i].a);
      }
      i = j;
    }
    for (std::size_t v = 0; v < sv.size(); ++v) {
      const auto &n = featureNbrs[v];
      if (n.empty()) continue;
      bool corner = n.size() != 2;
      if (!corner) {
        const Vec3d da = normalized(sv[n[0]] - sv[v]), db = normalized(sv[n[1]] - sv[v]);
        corner = dot(da, db) > -cosThr;  // the feature line turns sharply
      }
      if (corner) corners.push_back(static_cast<int>(v));
    }
  }
  st.featureEdges = segs.size();
  st.featureCorners = corners.size();

  // --- smoothing ---
  // Interior vertices move towards the centroid of their neighbours, boundary
  // vertices towards the centroid of their boundary neighbours and back onto
  // the surface. A move is kept only if the worst tet around gets better.
  auto smoothVertex = [&](int v) {
    if (fixed[v]) return false;
    Vec3d c{0.0, 0.0, 0.0};
    int n = 0;
    if (boundary[v]) {
      for (int u : boundaryNbrs[v]) {
        c += pos[u];
        ++n;
      }
    } else {
      for (int i = tetStart[v]; i < tetStart[v + 1]; ++i)
        for (int k = 0; k < 4; ++k) {
          const int u = tets[tetList[i]][k];
          if (u == v) continue;
          c += pos[u];
          ++n;
        }
    }
    if (n == 0) return false;
    c = c * (1.0 / n);
    const double before = minQualityAround(v, pos[v]);
    for (double w : {1.0, 0.5, 0.25}) {
      Vec3d target = pos[v] + (c - pos[v]) * w;
      if (boundary[v]) {
        target = sdf.closestPoint(target);
        const double move = length(target - pos[v]);
        if (move > 0.5 * meanLen[v]) continue;
        // the closest point may be on another part across a thin gap
        if (!sdf.segmentOnSide(pos[v], target, 1, 0.1 * move)) continue;
      }
      if (minQualityAround(v, target) > before + 1e-9) {
        pos[v] = target;
        return true;
      }
    }
    return false;
  };
  auto smoothBadTets = [&]() {
    std::vector<int> verts;
    for (int ti : tetsBelow(kSmoothTarget))
      for (int k = 0; k < 4; ++k) verts.push_back(tets[ti][k]);
    std::sort(verts.begin(), verts.end());
    verts.erase(std::unique(verts.begin(), verts.end()), verts.end());
    std::size_t moved = 0;
    for (int v : verts)
      if (smoothVertex(v)) ++moved;
    st.smoothed += moved;
    return moved;
  };

  // --- feature snapping ---
  if (!segs.empty()) {
    const double protectRadius = std::max(snapStrength, 0.5);
    const double maxRadius = protectRadius * maxMeanLen;
    Vec3d origin = sv[0];
    for (const Vec3d &p : sv)
      origin = {std::min(origin.x, p.x), std::min(origin.y, p.y), std::min(origin.z, p.z)};

    // Feature segments are split into pieces no longer than a grid cell, so
    // a piece covers few cells and every piece within maxRadius is found.
    HashGrid segGrid(origin, maxRadius);
    for (std::size_t s = 0; s < segs.size(); ++s) {
      const Vec3d a = sv[segs[s].first], b = sv[segs[s].second];
      const int pieces = std::max(1, static_cast<int>(std::ceil(length(b - a) / maxRadius)));
      for (int k = 0; k < pieces; ++k) {
        const Vec3d p0 = a + (b - a) * (double(k) / pieces);
        const Vec3d p1 = a + (b - a) * (double(k + 1) / pieces);
        segGrid.insert({std::min(p0.x, p1.x), std::min(p0.y, p1.y), std::min(p0.z, p1.z)},
                       {std::max(p0.x, p1.x), std::max(p0.y, p1.y), std::max(p0.z, p1.z)},
                       static_cast<int>(s));
      }
    }
    HashGrid cornerGrid(origin, maxRadius);
    for (std::size_t c = 0; c < corners.size(); ++c)
      cornerGrid.insert(sv[corners[c]], static_cast<int>(c));

    struct Cand {
      int v;
      int corner;  // index into corners, -1 if there is none in reach
      Vec3d cornerTarget, edgeTarget;
      double dist;
      bool hasEdge;
    };
    std::vector<Cand> slots(nv);
    std::vector<unsigned char> isCand(nv, 0);
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 256)
#endif
    for (int v = 0; v < nv; ++v) {
      if (!boundary[v]) continue;
      const Vec3d p = pos[v];
      const double reach = protectRadius * meanLen[v];
      const double r = snapStrength * meanLen[v];
      double nearest = std::numeric_limits<double>::max();
      Cand c{v, -1, {}, {}, std::numeric_limits<double>::max(), false};
      double bestCorner = r;
      cornerGrid.forNear(p, [&](int ci) {
        const double d = length(sv[corners[ci]] - p);
        nearest = std::min(nearest, d);
        if (d <= bestCorner) {
          bestCorner = d;
          c.corner = ci;
          c.cornerTarget = sv[corners[ci]];
        }
      });
      double bestEdge = r;
      segGrid.forNear(p, [&](int si) {
        const Vec3d q = closestOnSegment(p, sv[segs[si].first], sv[segs[si].second]);
        const double d = length(q - p);
        nearest = std::min(nearest, d);
        if (d <= bestEdge) {
          bestEdge = d;
          c.edgeTarget = q;
          c.hasEdge = true;
        }
      });
      if (nearest <= reach) fixed[v] = 1;
      if (c.corner < 0 && !c.hasEdge) continue;
      c.dist = c.corner >= 0 ? bestCorner : bestEdge;
      slots[v] = c;
      isCand[v] = 1;
    }
    std::vector<Cand> cands;
    for (int v = 0; v < nv; ++v)
      if (isCand[v]) cands.push_back(slots[v]);
    slots.clear();
    slots.shrink_to_fit();
    st.snapCandidates = cands.size();
    // corners first, then the vertices closest to their feature
    std::sort(cands.begin(), cands.end(), [](const Cand &x, const Cand &y) {
      if ((x.corner >= 0) != (y.corner >= 0)) return x.corner >= 0;
      return x.dist < y.dist;
    });

    std::vector<unsigned char> cornerTaken(corners.size(), 0);
    HashGrid snappedGrid(origin, maxRadius);
    // every tetrahedron stays positive and above the floor (or no worse)
    auto snapIsValid = [&](int v, const Vec3d &target) {
      // the feature may belong to another part across a thin gap
      if (!sdf.segmentOnSide(pos[v], target, 1, 0.1 * length(target - pos[v])))
        return false;
      for (int i = tetStart[v]; i < tetStart[v + 1]; ++i) {
        const auto &t = tets[tetList[i]];
        Vec3d q[4];
        for (int k = 0; k < 4; ++k) q[k] = pos[t[k]];
        const double qOld = tetQuality(q[0], q[1], q[2], q[3]);
        for (int k = 0; k < 4; ++k)
          if (t[k] == v) q[k] = target;
        const double qNew = tetQuality(q[0], q[1], q[2], q[3]);
        if (!(qNew > 0.0) || qNew < std::min(kQualityFloor, qOld)) return false;
      }
      // keep snapped vertices apart, they may not share a tetrahedron
      const double sep = 0.25 * meanLen[v];
      bool apart = true;
      snappedGrid.forNear(target, [&](int u) {
        if (u != v && length(pos[u] - target) < sep) apart = false;
      });
      return apart;
    };
    auto trySnap = [&](int v, const Vec3d &target) {
      if (!snapIsValid(v, target)) return false;
      pos[v] = target;
      snappedGrid.insert(target, v);
      return true;
    };

    std::vector<Cand> pending = std::move(cands);
    for (int round = 0; round < kSnapRounds && !pending.empty(); ++round) {
      std::vector<Cand> rejected;
      for (const Cand &c : pending) {
        bool done = false;
        if (c.corner >= 0 && !cornerTaken[c.corner]) {
          done = trySnap(c.v, c.cornerTarget);
          if (done) cornerTaken[c.corner] = 1;
        }
        if (!done && c.hasEdge) done = trySnap(c.v, c.edgeTarget);
        if (done)
          ++st.snapped;
        else
          rejected.push_back(c);
      }
      pending = std::move(rejected);
      // relaxing the neighbourhood may make room for a retry
      if (!pending.empty() && round + 1 < kSnapRounds) smoothBadTets();
    }
  }

  for (int pass = 0; pass < kSmoothPasses; ++pass)
    if (smoothBadTets() == 0) break;

  // How far a boundary triangle is from the input surface.
  auto faceError = [&](const Vec3d &a, const Vec3d &b, const Vec3d &c) {
    const Vec3d s[4] = {(a + b + c) * (1.0 / 3.0), (a + b) * 0.5, (b + c) * 0.5,
                        (c + a) * 0.5};
    double e = 0.0;
    for (const Vec3d &p : s) e = std::max(e, std::fabs(sdf.signedDistance(p)));
    return e;
  };

  // --- feature point insertion ---
  // Where no boundary vertex was close enough to a sharp edge or corner to be
  // snapped onto it, the boundary cuts the feature off (saw-tooth notches,
  // truncated corners). A new vertex on the feature is connected to the
  // boundary triangles it sees, filling the notch with a fan of tetrahedra.
  if (!segs.empty() && snapStrength > 0.0) {
    auto edgeKey = [](int a, int b) {
      if (a > b) std::swap(a, b);
      return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(a)) << 32) |
             static_cast<std::uint32_t>(b);
    };
    // outward oriented boundary faces
    std::vector<std::array<int, 3>> bf;
    {
      std::vector<std::pair<std::array<int, 3>, std::array<int, 3>>> recs;
      recs.reserve(tets.size() * 4);
      for (const auto &t : tets) {
        const std::array<int, 3> o[4] = {{t[1], t[2], t[3]},
                                         {t[0], t[3], t[2]},
                                         {t[0], t[1], t[3]},
                                         {t[0], t[2], t[1]}};
        for (const auto &f : o) {
          std::array<int, 3> k = f;
          std::sort(k.begin(), k.end());
          recs.push_back({k, f});
        }
      }
      std::sort(recs.begin(), recs.end(),
                [](const auto &x, const auto &y) { return x.first < y.first; });
      for (std::size_t i = 0; i < recs.size();) {
        std::size_t j = i + 1;
        while (j < recs.size() && recs[j].first == recs[i].first) ++j;
        if (j - i == 1) bf.push_back(recs[i].second);
        i = j;
      }
    }
    double maxEdge = 0.0, sumEdge = 0.0;
    Vec3d lo = pos[bf[0][0]];
    for (const auto &f : bf)
      for (int k = 0; k < 3; ++k) {
        const double l = length(pos[f[(k + 1) % 3]] - pos[f[k]]);
        maxEdge = std::max(maxEdge, l);
        sumEdge += l;
        const Vec3d &p = pos[f[k]];
        lo = {std::min(lo.x, p.x), std::min(lo.y, p.y), std::min(lo.z, p.z)};
      }
    const double meanEdge = sumEdge / (3.0 * bf.size());
    std::vector<unsigned char> alive(bf.size(), 1);
    HashGrid faceGrid(lo, maxEdge);
    std::unordered_map<std::uint64_t, std::vector<int>> edgeFaces;
    auto registerFace = [&](int id) {
      const auto &f = bf[id];
      Vec3d mn = pos[f[0]], mx = pos[f[0]];
      for (int k = 1; k < 3; ++k) {
        const Vec3d &p = pos[f[k]];
        mn = {std::min(mn.x, p.x), std::min(mn.y, p.y), std::min(mn.z, p.z)};
        mx = {std::max(mx.x, p.x), std::max(mx.y, p.y), std::max(mx.z, p.z)};
      }
      faceGrid.insert(mn, mx, id);
      for (int k = 0; k < 3; ++k) edgeFaces[edgeKey(f[k], f[(k + 1) % 3])].push_back(id);
    };
    for (int id = 0; id < static_cast<int>(bf.size()); ++id) registerFace(id);
    auto faceUnitNormal = [&](int id) {
      const auto &f = bf[id];
      return normalized(cross(pos[f[1]] - pos[f[0]], pos[f[2]] - pos[f[0]]));
    };
    auto faceDist = [&](const Vec3d &p, int id) {
      const auto &f = bf[id];
      return length(p - closestOnTriangle(p, pos[f[0]], pos[f[1]], pos[f[2]]));
    };
    auto faceLen = [&](int id) {
      const auto &f = bf[id];
      return (length(pos[f[1]] - pos[f[0]]) + length(pos[f[2]] - pos[f[1]]) +
              length(pos[f[0]] - pos[f[2]])) /
             3.0;
    };
    auto nearestFace = [&](const Vec3d &p, double &dist) {
      int best = -1;
      dist = std::numeric_limits<double>::max();
      faceGrid.forNear(p, [&](int id) {
        if (!alive[id]) return;
        const double d = faceDist(p, id);
        if (d < dist) {
          dist = d;
          best = id;
        }
      });
      return best;
    };

    std::vector<std::pair<Vec3d, bool>> targets;  // point, is corner
    for (int c : corners) targets.push_back({sv[c], true});
    for (const auto &s : segs) {
      const Vec3d a = sv[s.first], b = sv[s.second];
      const int n = std::max(1, static_cast<int>(std::ceil(length(b - a) / (0.25 * meanEdge))));
      for (int k = 1; k < n; ++k) targets.push_back({a + (b - a) * (double(k) / n), false});
    }
    std::vector<double> targetDist(targets.size(), -1.0);
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 64)
#endif
    for (int i = 0; i < static_cast<int>(targets.size()); ++i) {
      double d;
      const int f = nearestFace(targets[i].first, d);
      if (f >= 0 && d > 0.1 * faceLen(f)) targetDist[i] = d;
    }
    std::vector<std::pair<double, int>> order;
    for (int i = 0; i < static_cast<int>(targets.size()); ++i)
      if (targetDist[i] >= 0.0)
        order.push_back(
            {targets[i].second ? std::numeric_limits<double>::max() : targetDist[i], i});
    // corners first, then the worst covered feature points
    std::sort(order.begin(), order.end(), std::greater<>());


    for (const auto &o : order) {
      const Vec3d P = targets[o.second].first;
      double d;
      const int nearest = nearestFace(P, d);
      if (nearest < 0) continue;
      const double L = faceLen(nearest);
      if (d <= 0.1 * L) continue;  // covered by an earlier insertion
      const double reach = std::min(2.0 * L, maxEdge);
      const double visEps = 0.02 * L;
      auto visible = [&](int id) {
        if (!alive[id] || faceDist(P, id) > reach) return false;
        return dot(P - pos[bf[id][0]], faceUnitNormal(id)) > visEps;
      };
      // the nearest face may lie in the plane of a feature face; start from
      // the nearest one that P sees
      int seed = -1;
      double seedDist = std::numeric_limits<double>::max();
      faceGrid.forNear(P, [&](int id) {
        if (!visible(id)) return;
        const double fd = faceDist(P, id);
        if (fd < seedDist) {
          seedDist = fd;
          seed = id;
        }
      });
      if (seed < 0) continue;
      // the connected patch of boundary faces that P sees
      std::vector<int> patch{seed};
      std::set<int> inPatch{seed};
      for (std::size_t i = 0; i < patch.size(); ++i) {
        const auto &f = bf[patch[i]];
        for (int k = 0; k < 3; ++k)
          for (int nb : edgeFaces[edgeKey(f[k], f[(k + 1) % 3])])
            if (!inPatch.count(nb) && visible(nb)) {
              inPatch.insert(nb);
              patch.push_back(nb);
            }
      }
      // it must be a disk with a single boundary loop
      std::unordered_map<std::uint64_t, int> edgeUse;
      std::set<int> patchVerts;
      for (int id : patch)
        for (int k = 0; k < 3; ++k) {
          ++edgeUse[edgeKey(bf[id][k], bf[id][(k + 1) % 3])];
          patchVerts.insert(bf[id][k]);
        }
      std::vector<std::pair<int, int>> rim;  // directed as in the faces
      for (int id : patch)
        for (int k = 0; k < 3; ++k) {
          const int a = bf[id][k], b = bf[id][(k + 1) % 3];
          if (edgeUse[edgeKey(a, b)] == 1) rim.push_back({a, b});
        }
      if (static_cast<long>(patchVerts.size()) - static_cast<long>(edgeUse.size()) +
              static_cast<long>(patch.size()) != 1)
        continue;
      std::unordered_map<int, int> next;
      bool simple = true;
      for (const auto &e : rim)
        if (!next.emplace(e.first, e.second).second) simple = false;
      if (!simple || rim.empty()) continue;
      std::size_t loopLen = 0;
      for (int v = rim[0].first;;) {
        const auto it = next.find(v);
        if (it == next.end()) break;
        v = it->second;
        if (++loopLen > rim.size() || v == rim[0].first) break;
      }
      if (loopLen != rim.size()) continue;

      // the fan must be valid, closer to the surface and not cut the mesh
      bool ok = true;
      for (int id : patch) {
        const auto &f = bf[id];
        if (tetQuality(pos[f[0]], pos[f[1]], pos[f[2]], P) < kInsertQuality) ok = false;
      }
      if (!ok) continue;
      double errOld = 0.0, errNew = 0.0;
      for (int id : patch)
        errOld = std::max(errOld, faceError(pos[bf[id][0]], pos[bf[id][1]], pos[bf[id][2]]));
      for (const auto &e : rim) errNew = std::max(errNew, faceError(pos[e.first], pos[e.second], P));
      if (!(errNew < 0.5 * errOld)) continue;
      for (int v : patchVerts) {
        faceGrid.forNear((P + pos[v]) * 0.5, [&](int id) {
          if (!ok || !alive[id] || inPatch.count(id)) return;
          const auto &f = bf[id];
          if (f[0] == v || f[1] == v || f[2] == v) return;
          if (segmentHitsTriangle(P, pos[v], pos[f[0]], pos[f[1]], pos[f[2]])) ok = false;
        });
        if (!ok) break;
      }
      if (!ok) continue;

      const int pi = static_cast<int>(pos.size());
      pos.push_back(P);
      for (int id : patch) {
        const auto &f = bf[id];
        tets.push_back({f[0], f[1], f[2], pi});
        alive[id] = 0;
      }
      for (const auto &e : rim) {
        bf.push_back({e.first, e.second, pi});
        alive.push_back(1);
        registerFace(static_cast<int>(bf.size()) - 1);
      }
      ++st.featurePointsInserted;
    }
  }
  if (st.featurePointsInserted > 0) {
    const int n = static_cast<int>(pos.size());
    tetStart.assign(n + 1, 0);
    for (const auto &t : tets)
      for (int k = 0; k < 4; ++k) ++tetStart[t[k] + 1];
    for (int v = 0; v < n; ++v) tetStart[v + 1] += tetStart[v];
  }

  // --- cap removal ---
  // Removing a tet with two boundary faces abc, abd swaps them for acd, bcd
  // (an edge flip ab -> cd on the surface); with three boundary faces the
  // fourth face becomes boundary. Both keep the boundary manifold as long as
  // the new boundary edges/faces are not boundary already.
  {
    auto faceKey = [](int a, int b, int c) {
      std::array<int, 3> f{a, b, c};
      std::sort(f.begin(), f.end());
      return f;
    };
    auto edgeKey = [](int a, int b) {
      if (a > b) std::swap(a, b);
      return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(a)) << 32) |
             static_cast<std::uint32_t>(b);
    };
    std::vector<std::array<int, 3>> allFaces;
    allFaces.reserve(tets.size() * 4);
    for (const auto &t : tets)
      for (int k = 0; k < 4; ++k)
        allFaces.push_back(faceKey(t[(k + 1) % 4], t[(k + 2) % 4], t[(k + 3) % 4]));
    std::sort(allFaces.begin(), allFaces.end());
    std::vector<std::array<int, 3>> bFaces;
    for (std::size_t i = 0; i < allFaces.size();) {
      std::size_t j = i + 1;
      while (j < allFaces.size() && allFaces[j] == allFaces[i]) ++j;
      if (j - i == 1) bFaces.push_back(allFaces[i]);
      i = j;
    }
    allFaces.clear();
    allFaces.shrink_to_fit();
    std::set<std::array<int, 3>> boundaryFaces(bFaces.begin(), bFaces.end());
    std::unordered_map<std::uint64_t, int> boundaryEdges;
    for (const auto &f : bFaces)
      for (int k = 0; k < 3; ++k) ++boundaryEdges[edgeKey(f[k], f[(k + 1) % 3])];
    auto edgeIsBoundary = [&](int a, int b) {
      const auto it = boundaryEdges.find(edgeKey(a, b));
      return it != boundaryEdges.end() && it->second > 0;
    };
    auto addFace = [&](const std::array<int, 3> &f, int sign) {
      if (sign > 0)
        boundaryFaces.insert(f);
      else
        boundaryFaces.erase(f);
      for (int k = 0; k < 3; ++k) boundaryEdges[edgeKey(f[k], f[(k + 1) % 3])] += sign;
    };

    std::vector<unsigned char> removed(tets.size(), 0);
    // positions do not change here, so the candidates can be found up front
    const std::vector<int> capCandidates = tetsBelow(kCapQuality);
    bool changed = true;
    while (changed) {
      changed = false;
      for (int ti : capCandidates) {
        if (removed[ti]) continue;
        const auto &t = tets[ti];
        std::array<int, 3> f[4];
        bool onBoundary[4];
        int nb = 0;
        for (int k = 0; k < 4; ++k) {  // face k is opposite to vertex k
          f[k] = faceKey(t[(k + 1) % 4], t[(k + 2) % 4], t[(k + 3) % 4]);
          onBoundary[k] = boundaryFaces.count(f[k]) != 0;
          nb += onBoundary[k];
        }
        if (nb == 2) {
          // boundary faces are opposite to c and d; they share the edge ab
          int c = -1, d = -1;
          for (int k = 0; k < 4; ++k)
            if (onBoundary[k]) (c < 0 ? c : d) = k;
          if (edgeIsBoundary(t[c], t[d])) continue;
        } else if (nb == 3) {
          int apex = -1;
          for (int k = 0; k < 4; ++k)
            if (!onBoundary[k]) apex = k;
          // the apex would otherwise be left as a non-manifold vertex
          if (tetStart[t[apex] + 1] - tetStart[t[apex]] != 1) continue;
        } else {
          continue;
        }
        // only flat caps: a tet filling a sharp edge or corner is thin, too,
        // but removing it would cut the feature off
        double errOld = 0.0, errNew = 0.0;
        double lrms = 0.0;
        for (int i = 0; i < 4; ++i)
          for (int j = i + 1; j < 4; ++j) lrms += dot(pos[t[j]] - pos[t[i]], pos[t[j]] - pos[t[i]]);
        lrms = std::sqrt(lrms / 6.0);
        for (int k = 0; k < 4; ++k) {
          const double e = faceError(pos[f[k][0]], pos[f[k][1]], pos[f[k][2]]);
          if (onBoundary[k])
            errOld = std::max(errOld, e);
          else
            errNew = std::max(errNew, e);
        }
        if (errNew > errOld + 0.02 * lrms) continue;
        for (int k = 0; k < 4; ++k) addFace(f[k], onBoundary[k] ? -1 : 1);
        removed[ti] = 1;
        ++st.capsRemoved;
        changed = true;
      }
    }
    if (st.capsRemoved > 0) {
      std::size_t n = 0;
      for (std::size_t ti = 0; ti < tets.size(); ++ti)
        if (!removed[ti]) tets[n++] = tets[ti];
      tets.resize(n);
    }
  }

  st.minQualityAfter = minQuality();
  return st;
}
