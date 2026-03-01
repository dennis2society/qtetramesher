/*
 * CutPointSnapper.cpp
 *
 * Extracts the interior tetrahedra from the BCC lattice after warping.
 *
 * For each lattice tet:
 *   - If all 4 vertices are inside/on-surface: output the tet as-is
 *   - If all 4 vertices are outside: skip
 *   - If mixed: compute cut points on crossing edges (projected to the
 *     actual surface mesh), then subdivide the tet into sub-tets using
 *     standard patterns (1-3, 2-2, 3-1 inside/outside splits).
 */

#include "CutPointSnapper.h"
#include <algorithm>
#include <cstdint>
#include <cmath>

uint64_t CutPointSnapper::edgeKey(unsigned int v0, unsigned int v1) {
  unsigned int lo = std::min(v0, v1);
  unsigned int hi = std::max(v0, v1);
  return ((uint64_t)lo << 32) | (uint64_t)hi;
}

int CutPointSnapper::classifyTet(const BCCLattice &lattice,
                                  const std::array<unsigned int, 4> &tet) {
  int mask = 0;
  for (int i = 0; i < 4; ++i) {
    if (lattice.sdfValues[tet[i]] <= 0.0f) {
      mask |= (1 << i);
    }
  }
  return mask;
}

unsigned int CutPointSnapper::findOrCreateCutVertex(
    unsigned int v0, unsigned int v1,
    const BCCLattice &lattice,
    const Warp &warp,
    const SignedDistanceField &sdf,
    std::unordered_map<uint64_t, unsigned int> &edgeCutMap,
    std::unordered_map<unsigned int, unsigned int> &vertexMap,
    std::vector<Vec3f> &outVertices) {

  // If one endpoint was warped to the surface (SDF=0), it IS the cut point.
  // Return that vertex's output index directly.
  bool v0OnSurface = (lattice.sdfValues[v0] == 0.0f);
  bool v1OnSurface = (lattice.sdfValues[v1] == 0.0f);
  bool v0Inside = (lattice.sdfValues[v0] < 0.0f);
  bool v1Inside = (lattice.sdfValues[v1] < 0.0f);

  if (v0OnSurface && (v1Inside || !v1OnSurface)) {
    // v0 is on the surface and is effectively the cut point
    auto it = vertexMap.find(v0);
    if (it != vertexMap.end()) return it->second;
    unsigned int idx = (unsigned int)outVertices.size();
    outVertices.push_back(lattice.vertices[v0]);
    vertexMap[v0] = idx;
    return idx;
  }
  if (v1OnSurface && (v0Inside || !v0OnSurface)) {
    auto it = vertexMap.find(v1);
    if (it != vertexMap.end()) return it->second;
    unsigned int idx = (unsigned int)outVertices.size();
    outVertices.push_back(lattice.vertices[v1]);
    vertexMap[v1] = idx;
    return idx;
  }

  // Check edge cache
  uint64_t key = edgeKey(v0, v1);
  auto it = edgeCutMap.find(key);
  if (it != edgeCutMap.end()) {
    return it->second;
  }

  // Compute cut point via linear interpolation then project to surface
  float s0 = lattice.sdfValues[v0];
  float s1 = lattice.sdfValues[v1];
  float absS0 = std::fabs(s0);
  float absS1 = std::fabs(s1);
  float denom = absS0 + absS1;
  float t = (denom > 1e-12f) ? (absS0 / denom) : 0.5f;

  Vec3f pos = lattice.vertices[v0] * (1.0f - t) + lattice.vertices[v1] * t;

  // Project onto the actual surface mesh for better accuracy
  pos = sdf.closestSurfacePoint(pos);

  unsigned int idx = (unsigned int)outVertices.size();
  outVertices.push_back(pos);
  edgeCutMap[key] = idx;
  return idx;
}

void CutPointSnapper::extractInteriorTets(
    BCCLattice &lattice,
    Warp &warp,
    const SignedDistanceField &sdf,
    std::vector<Vec3f> &outVertices,
    std::vector<Tetrahedron> &outTetras) {

  // Map from lattice vertex index to output vertex index
  std::unordered_map<unsigned int, unsigned int> vertexMap;
  // Map from edge key to output cut vertex index
  std::unordered_map<uint64_t, unsigned int> edgeCutMap;

  // Helper: get or create an output vertex for a lattice vertex
  auto getOutVertex = [&](unsigned int latticeIdx) -> unsigned int {
    auto it = vertexMap.find(latticeIdx);
    if (it != vertexMap.end()) return it->second;
    unsigned int idx = (unsigned int)outVertices.size();
    outVertices.push_back(lattice.vertices[latticeIdx]);
    vertexMap[latticeIdx] = idx;
    return idx;
  };

  // Helper: emit a tet (with degenerate tet filtering)
  auto emitTet = [&](unsigned int a, unsigned int b, unsigned int c,
                      unsigned int d) {
    // Skip degenerate tets where any two vertices are the same
    if (a == b || a == c || a == d || b == c || b == d || c == d) return;

    // Skip zero-volume tets
    Vec3f va = outVertices[a], vb = outVertices[b];
    Vec3f vc = outVertices[c], vd = outVertices[d];
    Vec3f e0 = vb - va, e1 = vc - va, e2 = vd - va;
    float vol = std::fabs(e0.dot(e1.cross(e2)));
    if (vol < 1e-14f) return;

    Tetrahedron t;
    t.index[0] = a;
    t.index[1] = b;
    t.index[2] = c;
    t.index[3] = d;
    outTetras.push_back(t);
  };

  for (const auto &tet : lattice.tets) {
    int mask = classifyTet(lattice, tet);

    if (mask == 0) {
      // All outside — skip
      continue;
    }

    if (mask == 0xF) {
      // All inside — emit directly
      unsigned int a = getOutVertex(tet[0]);
      unsigned int b = getOutVertex(tet[1]);
      unsigned int c = getOutVertex(tet[2]);
      unsigned int d = getOutVertex(tet[3]);
      emitTet(a, b, c, d);
      continue;
    }

    // Count inside vertices
    int insideIdx[4], outsideIdx[4];
    int nIn = 0, nOut = 0;
    for (int i = 0; i < 4; ++i) {
      if (mask & (1 << i)) {
        insideIdx[nIn++] = i;
      } else {
        outsideIdx[nOut++] = i;
      }
    }

    if (nIn == 1) {
      // 1 inside, 3 outside → 1 tet
      int in0 = insideIdx[0];
      unsigned int vi = getOutVertex(tet[in0]);
      unsigned int c0 = findOrCreateCutVertex(tet[in0], tet[outsideIdx[0]],
                                               lattice, warp, sdf, edgeCutMap, vertexMap, outVertices);
      unsigned int c1 = findOrCreateCutVertex(tet[in0], tet[outsideIdx[1]],
                                               lattice, warp, sdf, edgeCutMap, vertexMap, outVertices);
      unsigned int c2 = findOrCreateCutVertex(tet[in0], tet[outsideIdx[2]],
                                               lattice, warp, sdf, edgeCutMap, vertexMap, outVertices);
      emitTet(vi, c0, c1, c2);

    } else if (nIn == 2) {
      // 2 inside, 2 outside → triangular prism decomposed into 3 tets
      int in0 = insideIdx[0], in1 = insideIdx[1];
      int out0 = outsideIdx[0], out1 = outsideIdx[1];

      unsigned int vi0 = getOutVertex(tet[in0]);
      unsigned int vi1 = getOutVertex(tet[in1]);
      unsigned int c00 = findOrCreateCutVertex(tet[in0], tet[out0],
                                                lattice, warp, sdf, edgeCutMap, vertexMap, outVertices);
      unsigned int c01 = findOrCreateCutVertex(tet[in0], tet[out1],
                                                lattice, warp, sdf, edgeCutMap, vertexMap, outVertices);
      unsigned int c10 = findOrCreateCutVertex(tet[in1], tet[out0],
                                                lattice, warp, sdf, edgeCutMap, vertexMap, outVertices);
      unsigned int c11 = findOrCreateCutVertex(tet[in1], tet[out1],
                                                lattice, warp, sdf, edgeCutMap, vertexMap, outVertices);

      // Decompose the triangular prism (vi0, vi1, c00, c01, c10, c11)
      // into 3 tets using a robust decomposition:
      emitTet(vi0, vi1, c00, c01);
      emitTet(vi1, c00, c10, c01);
      emitTet(c01, c10, c00, c11);

    } else if (nIn == 3) {
      // 3 inside, 1 outside → symmetric to 1-inside case
      int out0 = outsideIdx[0];
      unsigned int vi0 = getOutVertex(tet[insideIdx[0]]);
      unsigned int vi1 = getOutVertex(tet[insideIdx[1]]);
      unsigned int vi2 = getOutVertex(tet[insideIdx[2]]);
      unsigned int c0 = findOrCreateCutVertex(tet[out0], tet[insideIdx[0]],
                                               lattice, warp, sdf, edgeCutMap, vertexMap, outVertices);
      unsigned int c1 = findOrCreateCutVertex(tet[out0], tet[insideIdx[1]],
                                               lattice, warp, sdf, edgeCutMap, vertexMap, outVertices);
      unsigned int c2 = findOrCreateCutVertex(tet[out0], tet[insideIdx[2]],
                                               lattice, warp, sdf, edgeCutMap, vertexMap, outVertices);

      // The interior is the original tet with one corner cut off.
      // 6 vertices: vi0, vi1, vi2, c0, c1, c2
      // Decompose into 3 tets:
      emitTet(vi0, vi1, vi2, c0);
      emitTet(vi1, vi2, c0, c1);
      emitTet(vi2, c0, c1, c2);
    }
  }
}
