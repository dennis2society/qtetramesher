/*
 * Warp.cpp
 *
 * Implements the warping / snapping rules from Shewchuk's paper.
 * For each edge of the BCC lattice that crosses the isosurface (sign change):
 *   - Compute the cut parameter t = |sdf(v0)| / (|sdf(v0)| + |sdf(v1)|)
 *   - If t < alpha, snap v0 to the cut point (warp v0 to surface)
 *   - If t > (1 - alpha), snap v1 to the cut point (warp v1 to surface)
 *   - Otherwise, the cut point remains as a new vertex
 *
 * The alpha threshold differs for "short" edges (body-center ↔ corner,
 * length = spacing * sqrt(3)/2) and "long" edges (corner ↔ corner
 * diagonals on faces, length = spacing * sqrt(2)).
 */

#include "Warp.h"
#include <cstdint>
#include <cmath>
#include <algorithm>

uint64_t Warp::edgeKey(unsigned int v0, unsigned int v1) const {
  unsigned int lo = std::min(v0, v1);
  unsigned int hi = std::max(v0, v1);
  return ((uint64_t)lo << 32) | (uint64_t)hi;
}

Warp::Warp(BCCLattice &lattice, float alphaS, float alphaL) {
  processAllEdges(lattice, alphaS, alphaL);
}

void Warp::processEdge(BCCLattice &lattice, unsigned int v0, unsigned int v1,
                        float alpha) {
  uint64_t key = edgeKey(v0, v1);
  if (processedEdges_.count(key)) return;
  processedEdges_.insert(key);

  float s0 = lattice.sdfValues[v0];
  float s1 = lattice.sdfValues[v1];

  // No sign change = no cut point on this edge
  if ((s0 > 0.0f && s1 > 0.0f) || (s0 < 0.0f && s1 < 0.0f)) return;
  // Both zero — already on surface
  if (s0 == 0.0f && s1 == 0.0f) return;

  // Compute cut parameter
  float absS0 = std::fabs(s0);
  float absS1 = std::fabs(s1);
  float t = absS0 / (absS0 + absS1);

  Vec3f cutPos = lattice.vertices[v0] * (1.0f - t) + lattice.vertices[v1] * t;

  if (t < alpha) {
    // Snap v0 to the cut point
    if (!warpedVertices.count(v0)) {
      lattice.vertices[v0] = cutPos;
      lattice.sdfValues[v0] = 0.0f;
      warpedVertices.insert(v0);
    }
  } else if (t > (1.0f - alpha)) {
    // Snap v1 to the cut point
    if (!warpedVertices.count(v1)) {
      lattice.vertices[v1] = cutPos;
      lattice.sdfValues[v1] = 0.0f;
      warpedVertices.insert(v1);
    }
  } else {
    // Cut point remains as a new vertex
    CutPoint cp;
    cp.edgeV0 = v0;
    cp.edgeV1 = v1;
    cp.t = t;
    cp.position = cutPos;
    cp.newVertexIndex = 0; // assigned later
    cp.snappedToV0 = false;
    cp.snappedToV1 = false;
    remainingCutPoints.push_back(cp);
  }
}

void Warp::processAllEdges(BCCLattice &lattice, float alphaS, float alphaL) {
  int ni = lattice.ni;
  int nj = lattice.nj;
  int nk = lattice.nk;

  // Iterate over each tet in the lattice and process its 6 edges
  for (const auto &tet : lattice.tets) {
    for (int e = 0; e < 4; ++e) {
      for (int f = e + 1; f < 4; ++f) {
        unsigned int v0 = tet[e];
        unsigned int v1 = tet[f];

        // Determine if this is a "short" edge (body-center to corner)
        // or a "long" edge (corner to corner / face diagonal).
        // Body centers have indices >= numCorners
        int numCorners = ni * nj * nk;
        bool v0IsCenter = (v0 >= (unsigned int)numCorners);
        bool v1IsCenter = (v1 >= (unsigned int)numCorners);

        float alpha;
        if (v0IsCenter || v1IsCenter) {
          // Short edge: body center to corner
          alpha = alphaS;
        } else {
          // Long edge: corner to corner (face diagonal)
          alpha = alphaL;
        }
        processEdge(lattice, v0, v1, alpha);
      }
    }
  }
}
