/*
 * TetraMeshQuality.cpp
 */

#include "TetraMeshQuality.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {

struct V3 {
  double x, y, z;
};
V3 sub(const V3 &a, const V3 &b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
V3 crs(const V3 &a, const V3 &b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
double dt(const V3 &a, const V3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
V3 toV3(const Vec3f &v) { return {v.x, v.y, v.z}; }

}  // namespace

double TetraTools::ComputeSurfaceVolume(const std::vector<Vec3f> &verts,
                                        const std::vector<Triangle> &tris) {
  double vol = 0.0;
  for (const Triangle &t : tris) {
    if (t.index[0] >= verts.size() || t.index[1] >= verts.size() ||
        t.index[2] >= verts.size())
      continue;
    const V3 a = toV3(verts[t.index[0]]), b = toV3(verts[t.index[1]]),
             c = toV3(verts[t.index[2]]);
    vol += dt(a, crs(b, c));
  }
  return vol / 6.0;
}

TetraTools::TetraMeshQuality TetraTools::ComputeTetraMeshQuality(
    const std::vector<Vec3f> &verts, const std::vector<Tetrahedron> &tetras,
    const std::vector<Vec3f> &surfVerts, const std::vector<Triangle> &surfTris) {
  TetraMeshQuality q;
  q.numTetras = tetras.size();
  q.numVertices = verts.size();
  if (!surfTris.empty()) q.surfaceVolume = ComputeSurfaceVolume(surfVerts, surfTris);
  if (tetras.empty()) return q;

  const double inf = std::numeric_limits<double>::max();
  double minQ = inf, sumQ = 0.0, minDih = inf, maxDih = 0.0;
  double minE2 = inf, maxE2 = 0.0;
  const double pi = 3.14159265358979323846;
  for (const Tetrahedron &t : tetras) {
    V3 p[4];
    bool valid = true;
    for (int k = 0; k < 4; ++k) {
      if (t.index[k] >= verts.size()) valid = false;
      else p[k] = toV3(verts[t.index[k]]);
    }
    if (!valid) continue;
    const V3 e[6] = {sub(p[1], p[0]), sub(p[2], p[0]), sub(p[3], p[0]),
                     sub(p[2], p[1]), sub(p[3], p[1]), sub(p[3], p[2])};
    double s2 = 0.0;
    for (const V3 &v : e) {
      const double l2 = dt(v, v);
      s2 += l2;
      minE2 = std::min(minE2, l2);
      maxE2 = std::max(maxE2, l2);
    }
    const double vol = dt(e[0], crs(e[1], e[2])) / 6.0;
    q.tetraVolume += vol;
    if (vol <= 0.0) ++q.numInvertedTetras;
    const double lrms = std::sqrt(s2 / 6.0);
    const double quality =
        lrms > 0.0 ? 6.0 * std::sqrt(2.0) * vol / (lrms * lrms * lrms) : 0.0;
    minQ = std::min(minQ, quality);
    sumQ += quality;
    if (quality < TetraMeshQuality::kPoorQuality) ++q.numPoorTetras;

    // outward face normals; face k is opposite to vertex k
    V3 n[4];
    bool degenerate = false;
    for (int k = 0; k < 4; ++k) {
      const V3 &a = p[(k + 1) % 4], &b = p[(k + 2) % 4], &c = p[(k + 3) % 4];
      V3 nk = crs(sub(b, a), sub(c, a));
      if (dt(nk, sub(p[k], a)) > 0.0) nk = {-nk.x, -nk.y, -nk.z};
      const double l = std::sqrt(dt(nk, nk));
      if (!(l > 0.0)) {
        degenerate = true;
        break;
      }
      n[k] = {nk.x / l, nk.y / l, nk.z / l};
    }
    if (degenerate) {
      minDih = 0.0;
      maxDih = 180.0;
      continue;
    }
    for (int i = 0; i < 4; ++i)
      for (int j = i + 1; j < 4; ++j) {
        const double c = std::min(1.0, std::max(-1.0, -dt(n[i], n[j])));
        const double angle = std::acos(c) * 180.0 / pi;
        minDih = std::min(minDih, angle);
        maxDih = std::max(maxDih, angle);
      }
  }
  q.minQuality = minQ == inf ? 0.0 : minQ;
  q.meanQuality = sumQ / tetras.size();
  q.minDihedralDeg = minDih == inf ? 0.0 : minDih;
  q.maxDihedralDeg = maxDih;
  q.minEdgeLength = minE2 == inf ? 0.0 : std::sqrt(minE2);
  q.maxEdgeLength = std::sqrt(maxE2);
  return q;
}
