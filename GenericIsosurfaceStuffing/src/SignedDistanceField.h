/*
 * SignedDistanceField.h
 *
 * Computes a signed distance field from a triangle mesh using:
 *   - Baerentzen & Aanaes pseudo-normal method for robust sign determination
 *   - Uniform grid spatial hashing for O(1) closest-triangle queries
 *   - Feature classification (face/edge/vertex) for correct pseudo-normals
 */

#ifndef SIGNED_DISTANCE_FIELD_H
#define SIGNED_DISTANCE_FIELD_H

#include "TetraMeshTools/GeometryTypes.h"
#include <cstdint>
#include <cmath>
#include <vector>
#include <unordered_map>

class SignedDistanceField {
public:
  SignedDistanceField(const std::vector<Vec3f> &vertices,
                      const std::vector<Triangle> &triangles);

  /**
   * Evaluate the signed distance at a given point.
   * Negative = inside, Positive = outside.
   */
  float evaluate(const Vec3f &p) const;

  /**
   * Return the closest point on the surface mesh to p.
   */
  Vec3f closestSurfacePoint(const Vec3f &p) const;

private:
  const std::vector<Vec3f> &verts_;
  const std::vector<Triangle> &tris_;

  // Closest-point feature classification
  enum Feature {
    FEAT_FACE,
    FEAT_EDGE_01, // edge between vertex 0 and vertex 1
    FEAT_EDGE_12, // edge between vertex 1 and vertex 2
    FEAT_EDGE_20, // edge between vertex 2 and vertex 0
    FEAT_VERT_0,
    FEAT_VERT_1,
    FEAT_VERT_2
  };

  struct ClosestResult {
    float distSq;
    Vec3f point;
    unsigned int triIdx;
    Feature feature;
  };

  // Precomputed normals for pseudo-normal sign determination
  std::vector<Vec3f> faceNormals_;          // per triangle (unit)
  std::vector<Vec3f> vertexPseudoNormals_;  // per vertex (angle-weighted)
  std::unordered_map<uint64_t, Vec3f> edgePseudoNormals_; // per edge

  // Uniform grid spatial hash for acceleration
  Vec3f gridMin_;
  float cellSize_;
  int gx_, gy_, gz_;
  std::vector<std::vector<unsigned int>> grid_;

  void precompute();
  void buildGrid();

  Vec3f closestPointOnTriangle(const Vec3f &p, unsigned int triIdx,
                                float &distSq, Feature &feat) const;
  ClosestResult findClosest(const Vec3f &p) const;
  Vec3f getPseudoNormal(unsigned int triIdx, Feature feat) const;

  int gridCellIndex(int ix, int iy, int iz) const;
  static uint64_t makeEdgeKey(unsigned int v0, unsigned int v1);
};

#endif // SIGNED_DISTANCE_FIELD_H
