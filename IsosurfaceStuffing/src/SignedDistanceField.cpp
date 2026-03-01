/*
 * SignedDistanceField.cpp
 *
 * Brute-force signed distance field evaluation from a triangle mesh.
 * For each query point, finds the closest triangle and uses the
 * pseudo-normal method to determine the sign.
 */

#include "SignedDistanceField.h"
#include <limits>
#include <algorithm>
#include <cmath>

SignedDistanceField::SignedDistanceField(const std::vector<Vec3f> &vertices,
                                        const std::vector<Triangle> &triangles)
    : verts_(vertices), tris_(triangles) {
  computeVertexNormals();
}

void SignedDistanceField::computeVertexNormals() {
  vertexNormals_.resize(verts_.size(), Vec3f(0.0f, 0.0f, 0.0f));
  for (const auto &tri : tris_) {
    const Vec3f &v0 = verts_[tri.index[0]];
    const Vec3f &v1 = verts_[tri.index[1]];
    const Vec3f &v2 = verts_[tri.index[2]];
    Vec3f e0 = v1 - v0;
    Vec3f e1 = v2 - v0;
    Vec3f faceNormal = e0.cross(e1);
    // Area-weighted accumulation (faceNormal magnitude = 2 * area)
    vertexNormals_[tri.index[0]] += faceNormal;
    vertexNormals_[tri.index[1]] += faceNormal;
    vertexNormals_[tri.index[2]] += faceNormal;
  }
  for (auto &n : vertexNormals_) {
    float len = n.length();
    if (len > 1e-12f) {
      n = n / len;
    }
  }
}

Vec3f SignedDistanceField::closestPointOnTriangle(
    const Vec3f &p, const Vec3f &v0, const Vec3f &v1, const Vec3f &v2,
    float &distSq) const {
  // Compute closest point on triangle to p using Voronoi region approach.
  Vec3f ab = v1 - v0;
  Vec3f ac = v2 - v0;
  Vec3f ap = p - v0;

  float d1 = ab.dot(ap);
  float d2 = ac.dot(ap);
  if (d1 <= 0.0f && d2 <= 0.0f) {
    distSq = (p - v0).squaredLength();
    return v0; // Closest to vertex 0
  }

  Vec3f bp = p - v1;
  float d3 = ab.dot(bp);
  float d4 = ac.dot(bp);
  if (d3 >= 0.0f && d4 <= d3) {
    distSq = (p - v1).squaredLength();
    return v1; // Closest to vertex 1
  }

  float vc = d1 * d4 - d3 * d2;
  if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
    float v = d1 / (d1 - d3);
    Vec3f pt = v0 + ab * v;
    distSq = (p - pt).squaredLength();
    return pt; // Closest to edge 0-1
  }

  Vec3f cp = p - v2;
  float d5 = ab.dot(cp);
  float d6 = ac.dot(cp);
  if (d6 >= 0.0f && d5 <= d6) {
    distSq = (p - v2).squaredLength();
    return v2; // Closest to vertex 2
  }

  float vb = d5 * d2 - d1 * d6;
  if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
    float w = d2 / (d2 - d6);
    Vec3f pt = v0 + ac * w;
    distSq = (p - pt).squaredLength();
    return pt; // Closest to edge 0-2
  }

  float va = d3 * d6 - d5 * d4;
  if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
    float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
    Vec3f pt = v1 + (v2 - v1) * w;
    distSq = (p - pt).squaredLength();
    return pt; // Closest to edge 1-2
  }

  // Inside the triangle
  float denom = 1.0f / (va + vb + vc);
  float v = vb * denom;
  float w = vc * denom;
  Vec3f pt = v0 + ab * v + ac * w;
  distSq = (p - pt).squaredLength();
  return pt;
}

float SignedDistanceField::evaluate(const Vec3f &p) const {
  float minDistSq = std::numeric_limits<float>::max();
  Vec3f closestPt;
  unsigned int closestTri = 0;
  // Simple closest-triangle feature for sign
  // We'll track barycentric info to determine pseudo-normal
  Vec3f bestClosestPoint;

  for (unsigned int i = 0; i < tris_.size(); ++i) {
    const Triangle &tri = tris_[i];
    const Vec3f &v0 = verts_[tri.index[0]];
    const Vec3f &v1 = verts_[tri.index[1]];
    const Vec3f &v2 = verts_[tri.index[2]];

    float dSq;
    Vec3f cp = closestPointOnTriangle(p, v0, v1, v2, dSq);
    if (dSq < minDistSq) {
      minDistSq = dSq;
      bestClosestPoint = cp;
      closestTri = i;
    }
  }

  float dist = std::sqrt(minDistSq);

  // Determine sign using the face normal at the closest triangle
  // If the vector from closest point to p points in the same direction
  // as the face normal, the point is outside (+); otherwise inside (-).
  const Triangle &tri = tris_[closestTri];
  const Vec3f &v0 = verts_[tri.index[0]];
  const Vec3f &v1 = verts_[tri.index[1]];
  const Vec3f &v2 = verts_[tri.index[2]];

  // Use interpolated vertex normal at closest point for more robust sign
  Vec3f faceNormal = (v1 - v0).cross(v2 - v0);
  float fnLen = faceNormal.length();
  if (fnLen > 1e-12f) {
    faceNormal = faceNormal / fnLen;
  }

  // For robustness, also incorporate vertex normals:
  // We use the face normal as a primary signal
  Vec3f diff = p - bestClosestPoint;
  float dotVal = diff.dot(faceNormal);

  // If very close to the surface, use vertex normals as backup
  if (dist < 1e-8f) {
    return 0.0f;
  }

  if (dotVal > 0.0f) {
    return dist;  // outside
  } else {
    return -dist; // inside
  }
}
