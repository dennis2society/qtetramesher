/*
 * SignedDistanceField.h
 *
 * Computes an approximate signed distance field from a triangle mesh.
 * Uses closest-point queries on triangles with sign determined by
 * pseudo-normal method (angle-weighted vertex normals).
 */

#ifndef SIGNED_DISTANCE_FIELD_H
#define SIGNED_DISTANCE_FIELD_H

#include "TetraMeshTools/GeometryTypes.h"
#include <vector>
#include <cmath>

class SignedDistanceField {
public:
  /**
   * Build the signed distance field from a surface mesh.
   * Precomputes per-vertex normals for sign determination.
   */
  SignedDistanceField(const std::vector<Vec3f> &vertices,
                      const std::vector<Triangle> &triangles);

  /**
   * Evaluate the signed distance at a given point.
   * Negative = inside, Positive = outside.
   */
  float evaluate(const Vec3f &p) const;

private:
  const std::vector<Vec3f> &verts_;
  const std::vector<Triangle> &tris_;
  std::vector<Vec3f> vertexNormals_;

  /**
   * Compute closest point on triangle (v0,v1,v2) to point p.
   * Returns the closest point and sets distSq to the squared distance.
   */
  Vec3f closestPointOnTriangle(const Vec3f &p,
                               const Vec3f &v0, const Vec3f &v1, const Vec3f &v2,
                               float &distSq) const;

  void computeVertexNormals();
};

#endif // SIGNED_DISTANCE_FIELD_H
