/*
 * Warp.cpp
 *
 * Two-pass warping / snapping rules from Shewchuk's paper.
 *
 * Pass 1: Using original (pre-warp) positions and SDF values, identify all
 *         edges with sign changes, compute cut points, and determine which
 *         vertices should be warped. Cut points are projected onto the
 *         actual surface mesh for better accuracy.
 *
 * Pass 2: For each vertex targeted by multiple warps, pick the closest
 *         cut point. Apply all warps. Filter remaining (unsnapped) cut points.
 */

#include "Warp.h"
#include <cstdint>
#include <cmath>
#include <algorithm>

uint64_t Warp::edgeKey(unsigned int v0, unsigned int v1) {
  unsigned int lo = std::min(v0, v1);
  unsigned int hi = std::max(v0, v1);
  return ((uint64_t)lo << 32) | (uint64_t)hi;
}

Warp::Warp(BCCLattice &lattice, const SignedDistanceField &sdf,
           float alphaS, float alphaL) {
  // Save original state before any modifications
  std::vector<Vec3f> origPositions = lattice.vertices;
  std::vector<float> origSDF = lattice.sdfValues;

  // Pass 1: Collect all warp candidates and pending cut points
  std::vector<WarpCandidate> candidates;
  std::vector<PendingCutPoint> pendingCuts;
  collectEdges(lattice, origPositions, origSDF, sdf,
               alphaS, alphaL, candidates, pendingCuts);

  // Pass 2: For each vertex, pick the closest warp candidate
  std::unordered_map<unsigned int, WarpCandidate> bestWarps;
  for (const auto &cand : candidates) {
    auto it = bestWarps.find(cand.vertex);
    if (it == bestWarps.end() || cand.distSq < it->second.distSq) {
      bestWarps[cand.vertex] = cand;
    }
  }

  // Apply warps to the lattice
  for (const auto &[vertIdx, cand] : bestWarps) {
    lattice.vertices[vertIdx] = cand.cutPosition;
    lattice.sdfValues[vertIdx] = 0.0f;
    warpedVertices.insert(vertIdx);
  }

  // Filter pending cut points: keep only those on edges where neither
  // endpoint was warped (warped endpoints serve as their own cut points)
  for (const auto &pc : pendingCuts) {
    if (!warpedVertices.count(pc.v0) && !warpedVertices.count(pc.v1)) {
      CutPoint cp;
      cp.edgeV0 = pc.v0;
      cp.edgeV1 = pc.v1;
      cp.t = pc.t;
      cp.position = pc.position;
      cp.newVertexIndex = 0;
      cp.snappedToV0 = false;
      cp.snappedToV1 = false;
      remainingCutPoints.push_back(cp);
    }
  }
}

void Warp::collectEdges(const BCCLattice &lattice,
                        const std::vector<Vec3f> &origPos,
                        const std::vector<float> &origSDF,
                        const SignedDistanceField &sdf,
                        float alphaS, float alphaL,
                        std::vector<WarpCandidate> &candidates,
                        std::vector<PendingCutPoint> &pendingCuts) {
  for (const auto &tet : lattice.tets) {
    for (int e = 0; e < 4; ++e) {
      for (int f = e + 1; f < 4; ++f) {
        unsigned int v0 = tet[e];
        unsigned int v1 = tet[f];

        uint64_t key = edgeKey(v0, v1);
        if (processedEdges_.count(key)) continue;
        processedEdges_.insert(key);

        float s0 = origSDF[v0];
        float s1 = origSDF[v1];

        // No sign change → no cut point
        if ((s0 > 0.0f && s1 > 0.0f) || (s0 < 0.0f && s1 < 0.0f)) continue;
        if (s0 == 0.0f && s1 == 0.0f) continue;

        // Determine edge type for alpha threshold
        bool v0IsCenter = lattice.isBodyCenter[v0];
        bool v1IsCenter = lattice.isBodyCenter[v1];
        float alpha = (v0IsCenter || v1IsCenter) ? alphaS : alphaL;

        // Compute cut parameter from original SDF values
        float absS0 = std::fabs(s0);
        float absS1 = std::fabs(s1);
        float t = absS0 / (absS0 + absS1);

        // Compute initial cut position via linear interpolation
        Vec3f cutPos = origPos[v0] * (1.0f - t) + origPos[v1] * t;

        // Project the cut point onto the actual surface mesh for accuracy
        cutPos = sdf.closestSurfacePoint(cutPos);

        if (t < alpha) {
          // Warp v0 to the cut point
          float distSq = (cutPos - origPos[v0]).squaredLength();
          candidates.push_back({v0, cutPos, distSq});
        } else if (t > (1.0f - alpha)) {
          // Warp v1 to the cut point
          float distSq = (cutPos - origPos[v1]).squaredLength();
          candidates.push_back({v1, cutPos, distSq});
        } else {
          // Cut point stays as a new vertex
          pendingCuts.push_back({v0, v1, t, cutPos});
        }
      }
    }
  }
}
