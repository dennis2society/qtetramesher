/*
 * MeshTransform.cpp
 */

#include "MeshTransform.h"

#include <algorithm>
#include <cmath>
#include <limits>

TetraTools::MeshTransform::MeshTransform() : scale(1.0f) { UpdateMatrix(); }

void TetraTools::MeshTransform::SetOriginal(
    const std::vector<Vec3f> &vertices_) {
  original = vertices_;
  const float inf = std::numeric_limits<float>::max();
  Vec3f lo(inf, inf, inf), hi(-inf, -inf, -inf);
  for (const Vec3f &v : original) {
    for (unsigned int k = 0; k < 3; ++k) {
      lo[k] = std::min(lo[k], v[k]);
      hi[k] = std::max(hi[k], v[k]);
    }
  }
  pivot = original.empty() ? Vec3f() : (lo + hi) * 0.5f;
  Reset();
}

void TetraTools::MeshTransform::Clear() {
  original.clear();
  original.shrink_to_fit();
  pivot = Vec3f();
  Reset();
}

void TetraTools::MeshTransform::SetScale(float scale_) {
  if (!(scale_ > 0.0f) || !std::isfinite(scale_)) return;
  scale = scale_;
  UpdateMatrix();
}

void TetraTools::MeshTransform::SetRotation(float xDeg_, float yDeg_,
                                            float zDeg_) {
  rotation = Vec3f(xDeg_, yDeg_, zDeg_);
  UpdateMatrix();
}

void TetraTools::MeshTransform::Reset() {
  scale = 1.0f;
  rotation = Vec3f();
  UpdateMatrix();
}

bool TetraTools::MeshTransform::IsIdentity() const {
  return scale == 1.0f && rotation.x == 0.0f && rotation.y == 0.0f &&
         rotation.z == 0.0f;
}

void TetraTools::MeshTransform::UpdateMatrix() {
  const float d2r = 3.14159265358979323846f / 180.0f;
  const float cx = std::cos(rotation.x * d2r), sx = std::sin(rotation.x * d2r);
  const float cy = std::cos(rotation.y * d2r), sy = std::sin(rotation.y * d2r);
  const float cz = std::cos(rotation.z * d2r), sz = std::sin(rotation.z * d2r);
  // R = Rz * Ry * Rx
  const float r[3][3] = {
      {cz * cy, cz * sy * sx - sz * cx, cz * sy * cx + sz * sx},
      {sz * cy, sz * sy * sx + cz * cx, sz * sy * cx - cz * sx},
      {-sy, cy * sx, cy * cx}};
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) m[i][j] = r[i][j] * scale;
}

Vec3f TetraTools::MeshTransform::Apply(const Vec3f &v_) const {
  const Vec3f d = v_ - pivot;
  return Vec3f(pivot.x + m[0][0] * d.x + m[0][1] * d.y + m[0][2] * d.z,
               pivot.y + m[1][0] * d.x + m[1][1] * d.y + m[1][2] * d.z,
               pivot.z + m[2][0] * d.x + m[2][1] * d.y + m[2][2] * d.z);
}

void TetraTools::MeshTransform::Apply(std::vector<Vec3f> &out_) const {
  out_.resize(original.size());
  for (std::size_t i = 0; i < original.size(); ++i)
    out_[i] = Apply(original[i]);
}
