/*
 * TetStuffer.cpp
 *
 * Isosurface stuffing (Labelle & Shewchuk, SIGGRAPH 2007).
 *
 * The background mesh is a BCC lattice (optionally graded through an octree).
 * Lattice points are labelled inside/outside from an exact signed distance
 * field, the isosurface crossing of every lattice edge is found by root
 * finding, lattice points that sit too close to such a crossing are warped
 * onto it, and the inside part of every background tetrahedron is finally
 * emitted through the stuffing stencils.
 */

#include "TetStuffer.h"

#include "BCCLattice.h"
#include "MeshOptimizer.h"
#include "SignedDistanceField.h"

#include <TetraMeshTools/Vec3d.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

std::uint64_t edgeKey(int a, int b) {
  if (a > b) std::swap(a, b);
  return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(a)) << 32) |
         static_cast<std::uint64_t>(static_cast<std::uint32_t>(b));
}

/// Isosurface crossing on the segment ab, bracketed by opposite signs at the
/// ends. Illinois-regulated false position against the real distance field, so
/// the boundary follows the surface instead of the lattice's linear estimate.
double findCrossing(const SignedDistanceField &sdf, const Vec3d &a, const Vec3d &b,
                    double fa, double fb, double tol) {
  double lo = 0.0, hi = 1.0, flo = fa, fhi = fb;
  double t = 0.5;
  for (int it = 0; it < 24; ++it) {
    const double denom = flo - fhi;
    t = (std::fabs(denom) > 0.0) ? lo + (hi - lo) * (flo / denom) : 0.5 * (lo + hi);
    if (!(t > lo && t < hi)) t = 0.5 * (lo + hi);
    const double ft = sdf.signedDistance(a + (b - a) * t);
    if (std::fabs(ft) <= tol) return t;
    if ((ft < 0.0) == (flo < 0.0)) {
      lo = t;
      flo = ft;
      fhi *= 0.5;
    } else {
      hi = t;
      fhi = ft;
      flo *= 0.5;
    }
  }
  return t;
}

}  // namespace

void TetStuffer::stuff(const std::vector<Vec3f> &surfVerts,
                       const std::vector<Triangle> &surfTris, float gridSpacing,
                       float alphaShort, float alphaLong, float grading,
                       float featureAngle, float featureSnap,
                       std::vector<Vec3f> &outVerts,
                       std::vector<Tetrahedron> &outTets) {
  outVerts.clear();
  outTets.clear();
  if (surfVerts.empty() || surfTris.empty()) {
    std::cout << "IsosurfaceStuffing: empty input." << std::endl;
    return;
  }

  Vec3d bbMin{surfVerts[0].x, surfVerts[0].y, surfVerts[0].z};
  Vec3d bbMax = bbMin;
  for (const auto &v : surfVerts) {
    bbMin = {std::min(bbMin.x, (double)v.x), std::min(bbMin.y, (double)v.y),
             std::min(bbMin.z, (double)v.z)};
    bbMax = {std::max(bbMax.x, (double)v.x), std::max(bbMax.y, (double)v.y),
             std::max(bbMax.z, (double)v.z)};
  }

  double h = gridSpacing;
  const bool graded = grading > 0.0f;
  const double diag = length(bbMax - bbMin);
  if (!(h > 0.0)) h = diag / 20.0;
  // Keep the lattice from exploding on a badly scaled cell size; a graded
  // lattice only uses the finest cells near the surface.
  const double hMin = diag / (graded ? 800.0 : 400.0);
  if (h < hMin) {
    std::cout << "IsosurfaceStuffing: cell size " << h << " too small, clamped to "
              << hMin << "." << std::endl;
    h = hMin;
  }

  // Both endpoints of an edge may only be warped if the thresholds stay below
  // half the edge, otherwise two lattice points collapse onto one cut point.
  const double aShort = std::min(std::max((double)alphaShort, 0.0), 0.49);
  const double aLong = std::min(std::max((double)alphaLong, 0.0), 0.49);
  if (aShort != (double)alphaShort || aLong != (double)alphaLong) {
    std::cout << "IsosurfaceStuffing: alpha clamped to [0, 0.49] (short=" << aShort
              << ", long=" << aLong << ")." << std::endl;
  }

  const SignedDistanceField sdf(surfVerts, surfTris);
  if (sdf.empty()) {
    std::cout << "IsosurfaceStuffing: no usable triangles." << std::endl;
    return;
  }

  BCCLattice lattice;
  lattice.build(bbMin, bbMax, h, graded ? grading : 0.0, sdf);
  const std::vector<Vec3d> &latticePos = lattice.positions();
  const std::vector<BccTet> &latticeTets = lattice.tets();
  if (latticeTets.empty()) {
    std::cout << "IsosurfaceStuffing: empty background lattice." << std::endl;
    return;
  }
  std::cout << "IsosurfaceStuffing: cell size " << h
            << (graded ? " (graded " + std::to_string(grading) + ")" : " (uniform)")
            << ", " << lattice.cellCount()
            << " cells, " << latticePos.size() << " lattice points, "
            << latticeTets.size() << " background tetrahedra." << std::endl;

  const int nv = static_cast<int>(latticePos.size());
  std::vector<Vec3d> pos = latticePos;
  std::vector<double> phi(nv);
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
  for (int i = 0; i < nv; ++i) phi[i] = sdf.signedDistance(latticePos[i]);

  // +1 inside, -1 outside, 0 on the surface. Lattice points that already lie
  // on the surface must get the zero label here: treating them as inside would
  // fake a sign change against an outside neighbour and the root finder would
  // then be handed a bracket that holds no crossing.
  const double onSurfaceEps = 1e-6 * h;
  std::vector<int> lab(nv);
  std::size_t numOnSurface = 0;
  for (int i = 0; i < nv; ++i) {
    if (std::fabs(phi[i]) <= onSurfaceEps) {
      lab[i] = 0;
      ++numOnSurface;
    } else {
      lab[i] = (phi[i] < 0.0) ? 1 : -1;
    }
  }

  // Unique lattice edges.
  std::unordered_map<std::uint64_t, int> edgeMap;
  std::vector<int> edgeA, edgeB;
  edgeMap.reserve(latticeTets.size() * 4);
  auto edgeIndex = [&](int a, int b) {
    const std::uint64_t k = edgeKey(a, b);
    const auto it = edgeMap.find(k);
    if (it != edgeMap.end()) return it->second;
    const int idx = static_cast<int>(edgeA.size());
    edgeA.push_back(std::min(a, b));
    edgeB.push_back(std::max(a, b));
    edgeMap.emplace(k, idx);
    return idx;
  };
  for (const BccTet &t : latticeTets)
    for (int i = 0; i < 4; ++i)
      for (int j = i + 1; j < 4; ++j) edgeIndex(t.v[i], t.v[j]);

  // Cut points on sign-changing edges.
  const int ne = static_cast<int>(edgeA.size());
  std::vector<double> cutT(ne, -1.0);
  std::vector<Vec3d> cutP(ne);
  const double tol = 1e-6 * h;
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 256)
#endif
  for (int e = 0; e < ne; ++e) {
    const int u = edgeA[e], w = edgeB[e];
    if (lab[u] * lab[w] != -1) continue;
    const double t = findCrossing(sdf, pos[u], pos[w], phi[u], phi[w], tol);
    cutT[e] = t;
    cutP[e] = pos[u] + (pos[w] - pos[u]) * t;
  }

  // Warping: every lattice point moves onto the nearest cut point that lies
  // within alpha of it, which is what keeps the stencils free of slivers.
  // Cut points that practically coincide with a lattice point are always
  // snapped, otherwise they would become a second vertex at the same place and
  // tear the two tetrahedra sharing that face apart.
  const double snapEps = 1e-4 * h;
  std::vector<double> bestDist(nv, std::numeric_limits<double>::max());
  std::vector<int> bestEdge(nv, -1);
  for (int e = 0; e < ne; ++e) {
    if (cutT[e] < 0.0) continue;
    const int u = edgeA[e], w = edgeB[e];
    const bool isLong = lattice.isCenter(u) == lattice.isCenter(w);
    const double alpha = isLong ? aLong : aShort;
    const double len = length(pos[w] - pos[u]);
    const double tu = cutT[e], tw = 1.0 - cutT[e];
    if ((tu < alpha || tu * len < snapEps) && tu * len < bestDist[u]) {
      bestDist[u] = tu * len;
      bestEdge[u] = e;
    }
    if ((tw < alpha || tw * len < snapEps) && tw * len < bestDist[w]) {
      bestDist[w] = tw * len;
      bestEdge[w] = e;
    }
  }
  std::size_t numWarped = 0;
  for (int i = 0; i < nv; ++i) {
    if (bestEdge[i] < 0) continue;
    pos[i] = cutP[bestEdge[i]];
    lab[i] = 0;
    ++numWarped;
  }

  // A warped endpoint already lies on the surface, so its edges carry no cut.
  std::size_t numCuts = 0;
  for (int e = 0; e < ne; ++e) {
    if (cutT[e] < 0.0) continue;
    if (lab[edgeA[e]] * lab[edgeB[e]] != -1) {
      cutT[e] = -1.0;
      continue;
    }
    ++numCuts;
  }
  std::cout << "IsosurfaceStuffing: " << numWarped << " lattice points warped, "
            << numOnSurface << " already on the surface, " << numCuts
            << " cut points." << std::endl;

  // Output vertices are allocated lazily so unused lattice points are skipped.
  std::vector<Vec3d> outPos;
  std::vector<int> latticeOut(nv, -1);
  std::vector<int> cutOut(ne, -1);
  outPos.reserve(latticePos.size() / 2 + 1);
  auto vLattice = [&](int i) {
    if (latticeOut[i] < 0) {
      latticeOut[i] = static_cast<int>(outPos.size());
      outPos.push_back(pos[i]);
    }
    return latticeOut[i];
  };
  auto vCut = [&](int e) {
    if (cutOut[e] < 0) {
      cutOut[e] = static_cast<int>(outPos.size());
      outPos.push_back(cutP[e]);
    }
    return cutOut[e];
  };
  // The crossing of edge (a,b) with a inside: a itself once a was warped.
  auto crossing = [&](int a, int b) {
    if (lab[a] == 0) return vLattice(a);
    return vCut(edgeIndex(a, b));
  };

  const double volEps = 1e-9 * h * h * h;
  std::vector<std::array<int, 4>> rawTets;
  rawTets.reserve(latticeTets.size());
  auto emit = [&](int a, int b, int c, int d) {
    if (a == b || a == c || a == d || b == c || b == d || c == d) return;
    const Vec3d &A = outPos[a];
    const double vol =
        dot(outPos[b] - A, cross(outPos[c] - A, outPos[d] - A)) / 6.0;
    if (std::fabs(vol) < volEps) return;
    if (vol > 0.0)
      rawTets.push_back({a, b, c, d});
    else
      rawTets.push_back({a, b, d, c});
  };

  // Fan the inside polyhedron from its lowest-numbered vertex. Quadrilateral
  // faces are split across their lowest-numbered corner, which only depends on
  // the corner identities, so the two tets sharing such a face agree.
  std::vector<std::vector<int>> faces;
  auto fanFaces = [&]() {
    int apex = std::numeric_limits<int>::max();
    for (const auto &f : faces)
      for (int v : f) apex = std::min(apex, v);
    for (const auto &f : faces) {
      if (std::find(f.begin(), f.end(), apex) != f.end()) continue;
      if (f.size() == 3) {
        emit(apex, f[0], f[1], f[2]);
      } else {
        const int m = static_cast<int>(std::min_element(f.begin(), f.end()) - f.begin());
        emit(apex, f[m], f[(m + 1) % 4], f[(m + 2) % 4]);
        emit(apex, f[m], f[(m + 2) % 4], f[(m + 3) % 4]);
      }
    }
  };

  for (const BccTet &t : latticeTets) {
    int in[4], out[4];
    int nIn = 0, nOut = 0;
    int nZero = 0;
    for (int k = 0; k < 4; ++k) {
      if (lab[t.v[k]] >= 0) {
        in[nIn++] = t.v[k];
        if (lab[t.v[k]] == 0) ++nZero;
      } else {
        out[nOut++] = t.v[k];
      }
    }
    if (nIn == 0) continue;
    if (nZero == 4) {
      // Every corner sits on the surface; only the centroid decides.
      const Vec3d c = (pos[t.v[0]] + pos[t.v[1]] + pos[t.v[2]] + pos[t.v[3]]) * 0.25;
      if (sdf.signedDistance(c) > 0.0) continue;
    }

    faces.clear();
    if (nOut == 0) {
      const int a = vLattice(in[0]), b = vLattice(in[1]);
      const int c = vLattice(in[2]), d = vLattice(in[3]);
      faces.push_back({a, b, c});
      faces.push_back({a, b, d});
      faces.push_back({a, c, d});
      faces.push_back({b, c, d});
    } else if (nOut == 1) {
      const int d = out[0];
      const int a = vLattice(in[0]), b = vLattice(in[1]), c = vLattice(in[2]);
      const int pa = crossing(in[0], d), pb = crossing(in[1], d),
                pc = crossing(in[2], d);
      faces.push_back({a, b, c});
      faces.push_back({pa, pb, pc});
      faces.push_back({a, b, pb, pa});
      faces.push_back({b, c, pc, pb});
      faces.push_back({c, a, pa, pc});
    } else if (nOut == 2) {
      const int c0 = out[0], d0 = out[1];
      const int a = vLattice(in[0]), b = vLattice(in[1]);
      const int pac = crossing(in[0], c0), pad = crossing(in[0], d0);
      const int pbc = crossing(in[1], c0), pbd = crossing(in[1], d0);
      faces.push_back({a, b, pbc, pac});
      faces.push_back({a, b, pbd, pad});
      faces.push_back({a, pac, pad});
      faces.push_back({b, pbd, pbc});
      faces.push_back({pac, pbc, pbd, pad});
    } else {
      const int a = vLattice(in[0]);
      const int p0 = crossing(in[0], out[0]), p1 = crossing(in[0], out[1]),
                p2 = crossing(in[0], out[2]);
      faces.push_back({p0, p1, p2});
      faces.push_back({a, p0, p1});
      faces.push_back({a, p1, p2});
      faces.push_back({a, p2, p0});
    }
    fanFaces();
  }

  {
    const MeshOptimizerStats os = optimizeStuffedMesh(
        surfVerts, surfTris, sdf, featureAngle, featureSnap, outPos, rawTets);
    if (os.featureEdges > 0)
      std::cout << "IsosurfaceStuffing: " << os.featureEdges << " sharp edges, "
                << os.featureCorners << " corners; " << os.snapped << " of "
                << os.snapCandidates << " nearby boundary vertices snapped onto them."
                << std::endl;
    std::cout << "IsosurfaceStuffing: " << os.smoothed
              << " vertex moves to improve quality, " << os.capsRemoved
              << " flat boundary tets removed, worst tet quality "
              << os.minQualityBefore << " -> " << os.minQualityAfter << "."
              << std::endl;
  }

  // Drop the output vertices that no surviving tetrahedron refers to.
  std::vector<int> remap(outPos.size(), -1);
  for (const auto &t : rawTets)
    for (int k = 0; k < 4; ++k) remap[t[k]] = 0;
  for (std::size_t i = 0; i < remap.size(); ++i) {
    if (remap[i] != 0) continue;
    remap[i] = static_cast<int>(outVerts.size());
    outVerts.push_back(Vec3f(static_cast<float>(outPos[i].x),
                             static_cast<float>(outPos[i].y),
                             static_cast<float>(outPos[i].z)));
  }
  outTets.reserve(rawTets.size());
  for (const auto &t : rawTets) {
    Tetrahedron tet;
    for (int k = 0; k < 4; ++k) tet.index[k] = static_cast<unsigned int>(remap[t[k]]);
    outTets.push_back(tet);
  }

  std::cout << "IsosurfaceStuffing: " << outVerts.size() << " vertices, "
            << outTets.size() << " tetrahedra." << std::endl;
}
