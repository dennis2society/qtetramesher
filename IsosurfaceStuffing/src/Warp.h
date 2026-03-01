/*
 * Warp.h
 *
 * Implements the warping rules from Shewchuk's Isosurface Stuffing paper.
 * When a lattice vertex is close to the isosurface (but not exactly on it),
 * the vertex is "warped" (snapped) to the surface cut point to avoid
 * creating poorly-shaped tetrahedra.
 *
 * The key thresholds are:
 *   alpha_short (alpha_s): for short BCC edges (body-center to corner)
 *   alpha_long (alpha_l):  for long BCC edges (corner to corner)
 *
 * A cut point at parameter t along an edge is snapped to the nearer endpoint
 * if t < alpha or t > (1-alpha).
 */

#ifndef WARP_H
#define WARP_H

#include "BCCLattice.h"
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

/**
 * Information about a cut point on a lattice edge.
 */
struct CutPoint {
  unsigned int edgeV0;  // first vertex of the edge
  unsigned int edgeV1;  // second vertex of the edge
  float t;              // parameter along edge (0 = at v0, 1 = at v1)
  Vec3f position;       // interpolated position
  unsigned int newVertexIndex; // index assigned when emitting the vertex
  bool snappedToV0;     // if true, this cut point was snapped to v0
  bool snappedToV1;     // if true, this cut point was snapped to v1
};

/**
 * Applies the warping rules to the BCC lattice. Returns the set of vertices
 * that have been warped (snapped) to the surface.
 */
class Warp {
public:
  /**
   * Perform the warping analysis.
   *
   * @param lattice   The BCC lattice with SDF values filled in
   * @param alphaS    Short-edge threshold
   * @param alphaL    Long-edge threshold
   */
  Warp(BCCLattice &lattice, float alphaS, float alphaL);

  /**
   * Set of vertex indices that have been warped to the surface.
   * These vertices have their SDF value set to 0 and position moved.
   */
  std::unordered_set<unsigned int> warpedVertices;

  /**
   * Cut points on edges that were not snapped (remain as new vertices).
   */
  std::vector<CutPoint> remainingCutPoints;

private:
  void processEdge(BCCLattice &lattice, unsigned int v0, unsigned int v1,
                   float alpha);
  void processAllEdges(BCCLattice &lattice, float alphaS, float alphaL);

  // Track which edges have been processed
  std::unordered_set<uint64_t> processedEdges_;
  uint64_t edgeKey(unsigned int v0, unsigned int v1) const;
};

#endif // WARP_H
