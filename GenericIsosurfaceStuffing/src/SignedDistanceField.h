/*
 * SignedDistanceField.h
 *
 * Exact signed distance to a triangle surface mesh.
 *
 * Distance is computed with a BVH over the triangles; the sign comes from the
 * angle-weighted pseudo-normal of the closest feature (Baerentzen & Aanaes,
 * "Signed Distance Computation Using the Angle Weighted Pseudonormal", 2005),
 * which is exact for closed manifold meshes.
 */

#ifndef SIGNED_DISTANCE_FIELD_H
#define SIGNED_DISTANCE_FIELD_H

#include <TetraMeshTools/GeometryTypes.h>
#include <TetraMeshTools/Vec3d.h>

#include <cstdint>
#include <unordered_map>
#include <vector>

class SignedDistanceField {
public:
  SignedDistanceField(const std::vector<Vec3f> &verts,
                      const std::vector<Triangle> &tris);

  /// Negative inside the surface, positive outside.
  double signedDistance(const Vec3d &p) const;

  /// Closest point on the surface; undefined if the mesh is empty.
  Vec3d closestPoint(const Vec3d &p) const;

  /// Outward normal of the surface feature closest to p.
  Vec3d surfaceNormal(const Vec3d &p) const;

  /// True if the segment ab stays inside (side = +1) or outside (side = -1)
  /// of the surface, allowing a deviation of tol.
  bool segmentOnSide(const Vec3d &a, const Vec3d &b, int side, double tol) const;

  /// True if the segment leaves the solid by more than tol and re-enters it
  /// through a surface facing the one it left, i.e. it spans a gap between
  /// two parts (or across a hole).
  bool segmentCrossesGap(const Vec3d &a, const Vec3d &b, double tol) const;

  bool empty() const { return tris_.empty(); }

private:
  /// Which part of a triangle a closest point landed on.
  enum Feature { FACE = 0, VERT_A, VERT_B, VERT_C, EDGE_AB, EDGE_BC, EDGE_CA };

  struct Node {
    Vec3d bmin, bmax;
    int left, right;  ///< -1 for a leaf
    int start, count;
  };

  struct Hit {
    double distSq;
    Vec3d point;
    int tri;
    Feature feature;
  };

  std::vector<Vec3d> verts_;
  std::vector<Triangle> tris_;
  std::vector<Vec3d> faceNormals_;
  std::vector<Vec3d> vertexNormals_;
  std::unordered_map<std::uint64_t, Vec3d> edgeNormals_;

  std::vector<Node> nodes_;
  std::vector<int> order_;  ///< triangle indices, permuted by the BVH build

  void buildNormals();
  void buildBVH();
  int buildNode(int start, int count, int depth);
  void query(int node, const Vec3d &p, Hit &best) const;
  Vec3d pseudoNormal(int tri, Feature f) const;
};

#endif  // SIGNED_DISTANCE_FIELD_H
