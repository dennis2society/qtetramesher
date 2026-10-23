/*
 * SignedDistanceField.cpp
 */

#include "SignedDistanceField.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {

std::uint64_t edgeKey(unsigned int a, unsigned int b) {
  if (a > b) std::swap(a, b);
  return (static_cast<std::uint64_t>(a) << 32) | static_cast<std::uint64_t>(b);
}

/// Closest point of triangle abc to p (Ericson, Real-Time Collision Detection),
/// extended to report which feature the point lies on.
Vec3d closestPointOnTriangle(const Vec3d &p, const Vec3d &a, const Vec3d &b,
                             const Vec3d &c, int &feature) {
  const Vec3d ab = b - a;
  const Vec3d ac = c - a;
  const Vec3d ap = p - a;
  const double d1 = dot(ab, ap);
  const double d2 = dot(ac, ap);
  if (d1 <= 0.0 && d2 <= 0.0) {
    feature = 1;
    return a;
  }
  const Vec3d bp = p - b;
  const double d3 = dot(ab, bp);
  const double d4 = dot(ac, bp);
  if (d3 >= 0.0 && d4 <= d3) {
    feature = 2;
    return b;
  }
  const double vc = d1 * d4 - d3 * d2;
  if (vc <= 0.0 && d1 >= 0.0 && d3 <= 0.0) {
    feature = 4;
    return a + ab * (d1 / (d1 - d3));
  }
  const Vec3d cp = p - c;
  const double d5 = dot(ab, cp);
  const double d6 = dot(ac, cp);
  if (d6 >= 0.0 && d5 <= d6) {
    feature = 3;
    return c;
  }
  const double vb = d5 * d2 - d1 * d6;
  if (vb <= 0.0 && d2 >= 0.0 && d6 <= 0.0) {
    feature = 6;
    return a + ac * (d2 / (d2 - d6));
  }
  const double va = d3 * d6 - d5 * d4;
  if (va <= 0.0 && (d4 - d3) >= 0.0 && (d5 - d6) >= 0.0) {
    feature = 5;
    return b + (c - b) * ((d4 - d3) / ((d4 - d3) + (d5 - d6)));
  }
  const double denom = 1.0 / (va + vb + vc);
  feature = 0;
  return a + ab * (vb * denom) + ac * (vc * denom);
}

double distanceSqToBox(const Vec3d &p, const Vec3d &bmin, const Vec3d &bmax) {
  double d = 0.0;
  const double dx = std::max(std::max(bmin.x - p.x, 0.0), p.x - bmax.x);
  const double dy = std::max(std::max(bmin.y - p.y, 0.0), p.y - bmax.y);
  const double dz = std::max(std::max(bmin.z - p.z, 0.0), p.z - bmax.z);
  d = dx * dx + dy * dy + dz * dz;
  return d;
}

/// Interior angle of triangle abc at vertex a.
double cornerAngle(const Vec3d &a, const Vec3d &b, const Vec3d &c) {
  const Vec3d u = normalized(b - a);
  const Vec3d v = normalized(c - a);
  return std::acos(std::max(-1.0, std::min(1.0, dot(u, v))));
}

}  // namespace

SignedDistanceField::SignedDistanceField(const std::vector<Vec3f> &verts,
                                         const std::vector<Triangle> &tris) {
  verts_.reserve(verts.size());
  for (const auto &v : verts) {
    verts_.push_back({static_cast<double>(v.x), static_cast<double>(v.y),
                      static_cast<double>(v.z)});
  }
  // Degenerate triangles have no usable normal and would poison the sign test.
  tris_.reserve(tris.size());
  for (const auto &t : tris) {
    if (t.index[0] >= verts_.size() || t.index[1] >= verts_.size() ||
        t.index[2] >= verts_.size())
      continue;
    if (t.index[0] == t.index[1] || t.index[1] == t.index[2] ||
        t.index[0] == t.index[2])
      continue;
    const Vec3d n = cross(verts_[t.index[1]] - verts_[t.index[0]],
                          verts_[t.index[2]] - verts_[t.index[0]]);
    if (length(n) <= 0.0) continue;
    tris_.push_back(t);
  }
  if (tris_.empty()) return;
  buildNormals();
  buildBVH();
}

void SignedDistanceField::buildNormals() {
  faceNormals_.resize(tris_.size());
  vertexNormals_.assign(verts_.size(), Vec3d{0.0, 0.0, 0.0});
  for (std::size_t i = 0; i < tris_.size(); ++i) {
    const Triangle &t = tris_[i];
    const Vec3d &a = verts_[t.index[0]];
    const Vec3d &b = verts_[t.index[1]];
    const Vec3d &c = verts_[t.index[2]];
    faceNormals_[i] = normalized(cross(b - a, c - a));
    vertexNormals_[t.index[0]] += faceNormals_[i] * cornerAngle(a, b, c);
    vertexNormals_[t.index[1]] += faceNormals_[i] * cornerAngle(b, c, a);
    vertexNormals_[t.index[2]] += faceNormals_[i] * cornerAngle(c, a, b);
    for (int e = 0; e < 3; ++e) {
      const std::uint64_t k = edgeKey(t.index[e], t.index[(e + 1) % 3]);
      edgeNormals_[k] += faceNormals_[i];
    }
  }
  for (auto &n : vertexNormals_) n = normalized(n);
  for (auto &kv : edgeNormals_) kv.second = normalized(kv.second);
}

void SignedDistanceField::buildBVH() {
  order_.resize(tris_.size());
  for (std::size_t i = 0; i < order_.size(); ++i) order_[i] = static_cast<int>(i);
  nodes_.reserve(2 * tris_.size());
  buildNode(0, static_cast<int>(order_.size()), 0);
}

int SignedDistanceField::buildNode(int start, int count, int depth) {
  const int self = static_cast<int>(nodes_.size());
  nodes_.push_back(Node{});
  Vec3d bmin{std::numeric_limits<double>::max(),
             std::numeric_limits<double>::max(),
             std::numeric_limits<double>::max()};
  Vec3d bmax{-std::numeric_limits<double>::max(),
             -std::numeric_limits<double>::max(),
             -std::numeric_limits<double>::max()};
  for (int i = start; i < start + count; ++i) {
    const Triangle &t = tris_[order_[i]];
    for (int k = 0; k < 3; ++k) {
      const Vec3d &v = verts_[t.index[k]];
      bmin = {std::min(bmin.x, v.x), std::min(bmin.y, v.y), std::min(bmin.z, v.z)};
      bmax = {std::max(bmax.x, v.x), std::max(bmax.y, v.y), std::max(bmax.z, v.z)};
    }
  }

  const int kLeafSize = 8;
  const int kMaxDepth = 48;
  if (count <= kLeafSize || depth >= kMaxDepth) {
    nodes_[self] = Node{bmin, bmax, -1, -1, start, count};
    return self;
  }

  const Vec3d ext = bmax - bmin;
  const int axis = (ext.x >= ext.y && ext.x >= ext.z) ? 0 : (ext.y >= ext.z ? 1 : 2);
  auto centroid = [&](int triIdx, int ax) {
    const Triangle &t = tris_[triIdx];
    const Vec3d c = (verts_[t.index[0]] + verts_[t.index[1]] + verts_[t.index[2]]) *
                    (1.0 / 3.0);
    return ax == 0 ? c.x : (ax == 1 ? c.y : c.z);
  };
  const int mid = start + count / 2;
  std::nth_element(order_.begin() + start, order_.begin() + mid,
                   order_.begin() + start + count,
                   [&](int l, int r) { return centroid(l, axis) < centroid(r, axis); });

  const int left = buildNode(start, mid - start, depth + 1);
  const int right = buildNode(mid, start + count - mid, depth + 1);
  nodes_[self] = Node{bmin, bmax, left, right, 0, 0};
  return self;
}

void SignedDistanceField::query(int node, const Vec3d &p, Hit &best) const {
  const Node &n = nodes_[node];
  if (distanceSqToBox(p, n.bmin, n.bmax) >= best.distSq) return;
  if (n.left < 0) {
    for (int i = n.start; i < n.start + n.count; ++i) {
      const int ti = order_[i];
      const Triangle &t = tris_[ti];
      int feat = 0;
      const Vec3d q = closestPointOnTriangle(p, verts_[t.index[0]], verts_[t.index[1]],
                                             verts_[t.index[2]], feat);
      const Vec3d d = q - p;
      const double dsq = dot(d, d);
      if (dsq < best.distSq) {
        best.distSq = dsq;
        best.point = q;
        best.tri = ti;
        best.feature = static_cast<Feature>(feat);
      }
    }
    return;
  }
  // Visit the nearer child first so the far one is more likely to be culled.
  const int a = n.left;
  const int b = n.right;
  const double da = distanceSqToBox(p, nodes_[a].bmin, nodes_[a].bmax);
  const double db = distanceSqToBox(p, nodes_[b].bmin, nodes_[b].bmax);
  if (da <= db) {
    query(a, p, best);
    query(b, p, best);
  } else {
    query(b, p, best);
    query(a, p, best);
  }
}

Vec3d SignedDistanceField::pseudoNormal(int tri, Feature f) const {
  const Triangle &t = tris_[tri];
  switch (f) {
    case FACE:
      return faceNormals_[tri];
    case VERT_A:
      return vertexNormals_[t.index[0]];
    case VERT_B:
      return vertexNormals_[t.index[1]];
    case VERT_C:
      return vertexNormals_[t.index[2]];
    case EDGE_AB:
    case EDGE_BC:
    case EDGE_CA: {
      const int e = (f == EDGE_AB) ? 0 : (f == EDGE_BC ? 1 : 2);
      const auto it = edgeNormals_.find(edgeKey(t.index[e], t.index[(e + 1) % 3]));
      if (it != edgeNormals_.end()) return it->second;
      return faceNormals_[tri];
    }
  }
  return faceNormals_[tri];
}

Vec3d SignedDistanceField::closestPoint(const Vec3d &p) const {
  if (tris_.empty()) return p;
  Hit best{std::numeric_limits<double>::max(), p, 0, FACE};
  query(0, p, best);
  return best.point;
}

double SignedDistanceField::signedDistance(const Vec3d &p) const {
  if (tris_.empty()) return std::numeric_limits<double>::max();
  Hit best{std::numeric_limits<double>::max(), p, 0, FACE};
  query(0, p, best);
  const double dist = std::sqrt(best.distSq);
  if (dist <= 0.0) return 0.0;
  const Vec3d d = p - best.point;
  return dot(d, pseudoNormal(best.tri, best.feature)) >= 0.0 ? dist : -dist;
}

Vec3d SignedDistanceField::surfaceNormal(const Vec3d &p) const {
  if (tris_.empty()) return Vec3d{0.0, 0.0, 0.0};
  Hit best{std::numeric_limits<double>::max(), p, 0, FACE};
  query(0, p, best);
  return normalized(pseudoNormal(best.tri, best.feature));
}

// The field is 1-Lipschitz: from a point with value v <= tol, no point closer
// than tol - v can exceed tol, so that is a safe step. Segments running along
// the surface (v ~ 0) take about len / tol steps instead of crawling.
bool SignedDistanceField::segmentOnSide(const Vec3d &a, const Vec3d &b, int side,
                                        double tol) const {
  const double len = length(b - a);
  if (!(len > 0.0)) return true;
  const Vec3d dir = (b - a) * (1.0 / len);
  const double minStep = 1e-3 * len;
  for (double s = 0.0; s <= len;) {
    const double d = signedDistance(a + dir * s);
    const double v = side > 0 ? d : -d;
    if (v > tol) return false;
    s += std::max(tol - v, minStep);
  }
  return true;
}

bool SignedDistanceField::segmentCrossesGap(const Vec3d &a, const Vec3d &b,
                                            double tol) const {
  const double len = length(b - a);
  if (!(len > 0.0)) return false;
  const Vec3d dir = (b - a) * (1.0 / len);
  const double minStep = 1e-3 * len;
  double exitS = -1.0, entryS = -1.0;
  for (double s = 0.0; s <= len;) {
    const double d = signedDistance(a + dir * s);
    if (d > tol) {
      if (exitS < 0.0) exitS = s;
      entryS = s;
      // sphere tracing outside: approaches the entry sheet without passing it,
      // so the last outside sample is near the sheet it re-enters through
      s += std::max(d, minStep);
    } else {
      s += std::max(tol - d, minStep);
    }
  }
  if (exitS < 0.0 || entryS <= exitS) return false;
  // Leaving through one sheet and coming back through a sheet facing it, as
  // opposed to a chord that just bulges out of a concave surface.
  return dot(surfaceNormal(a + dir * exitS), surfaceNormal(a + dir * entryS)) < -0.3;
}
