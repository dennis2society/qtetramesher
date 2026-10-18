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

inline Vec3d operator+(const Vec3d &a, const Vec3d &b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Vec3d operator*(const Vec3d &a, double s) {
  return {a.x * s, a.y * s, a.z * s};
}

inline Vec3d operator*(double s, const Vec3d &a) { return a * s; }

inline Vec3d &operator+=(Vec3d &a, const Vec3d &b) {
  a.x += b.x;
  a.y += b.y;
  a.z += b.z;
  return a;
}

inline Vec3d cross(const Vec3d &a, const Vec3d &b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

inline double dot(const Vec3d &a, const Vec3d &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double length(const Vec3d &a) { return std::sqrt(dot(a, a)); }

inline Vec3d normalized(const Vec3d &a) {
  const double l = length(a);
  return l > 0.0 ? a * (1.0 / l) : Vec3d{0.0, 0.0, 0.0};
}

#endif  // _VEC3D_H
