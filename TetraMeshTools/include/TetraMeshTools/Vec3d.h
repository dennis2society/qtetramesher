/*
 * Vec3d.h
 *
 * Minimal double-precision 3D vector for geometric predicates where the
 * float-based Vec3f is not accurate enough.
 */

#ifndef _VEC3D_H
#define _VEC3D_H

#include <cmath>

struct Vec3d {
  double x, y, z;
};

inline Vec3d operator-(const Vec3d &a, const Vec3d &b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline Vec3d cross(const Vec3d &a, const Vec3d &b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

inline double dot(const Vec3d &a, const Vec3d &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double length(const Vec3d &a) { return std::sqrt(dot(a, a)); }

#endif  // _VEC3D_H
