/*
 * MeshTransform.cpp
 */

#include "MeshTransform.h"

#include <algorithm>
#include <cmath>
#include <limits>

TetraTools::MeshTransform::MeshTransform() : scale(1.0f) { UpdateMatrix(); }

void TetraTools::MeshTransform::SetOriginal(
    const std::vector<Vec3f> &vertices_, const std::vector<Vec3f> &normals_) {
  original = vertices_;
  originalNormals =
      normals_.size() == vertices_.size() ? normals_ : std::vector<Vec3f>();
  const float inf = std::numeric_limits<float>::max();
  Vec3f lo(inf, inf, inf), hi(-inf, -inf, -inf);
  for (const Vec3f &v : original) {
    for (unsigned int k = 0; k < 3; ++k) {
      lo[k] = std::min(lo[k], v[k]);
      hi[k] = std::max(hi[k], v[k]);
    }
  }
  if (original.empty()) {
    lo = Vec3f();
    hi = Vec3f();
  }
  originalBBox.min = lo;
  originalBBox.max = hi;
  pivot = (lo + hi) * 0.5f;
  Reset();
}

void TetraTools::MeshTransform::Clear() {
  original.clear();
  original.shrink_to_fit();
  originalNormals.clear();
  originalNormals.shrink_to_fit();
  originalBBox = BoundingBox();
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
  const float m[3][3] = {
      {cz * cy, cz * sy * sx - sz * cx, cz * sy * cx + sz * sx},
      {sz * cy, sz * sy * sx + cz * cx, sz * sy * cx - cz * sx},
      {-sy, cy * sx, cy * cx}};
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) r[i][j] = m[i][j];
}

Vec3f TetraTools::MeshTransform::Apply(const Vec3f &v_) const {
  const Vec3f d = (v_ - pivot) * scale;
  return Vec3f(pivot.x + r[0][0] * d.x + r[0][1] * d.y + r[0][2] * d.z,
               pivot.y + r[1][0] * d.x + r[1][1] * d.y + r[1][2] * d.z,
               pivot.z + r[2][0] * d.x + r[2][1] * d.y + r[2][2] * d.z);
}

void TetraTools::MeshTransform::Apply(std::vector<Vec3f> &out_) const {
  out_.resize(original.size());
  for (std::size_t i = 0; i < original.size(); ++i)
    out_[i] = Apply(original[i]);
}

void TetraTools::MeshTransform::ApplyToNormals(
    std::vector<Vec3f> &out_) const {
  out_.resize(originalNormals.size());
  for (std::size_t i = 0; i < originalNormals.size(); ++i) {
    const Vec3f &n = originalNormals[i];
    out_[i] = Vec3f(r[0][0] * n.x + r[0][1] * n.y + r[0][2] * n.z,
                    r[1][0] * n.x + r[1][1] * n.y + r[1][2] * n.z,
                    r[2][0] * n.x + r[2][1] * n.y + r[2][2] * n.z);
  }
}

BoundingBox TetraTools::MeshTransform::ComputeBoundingBox() const {
  BoundingBox bb;
  if (original.empty()) return bb;
  bb.min = Apply(original[0]);
  bb.max = bb.min;
  for (const Vec3f &o : original) {
    const Vec3f v = Apply(o);
    for (unsigned int k = 0; k < 3; ++k) {
      bb.min[k] = std::min(bb.min[k], v[k]);
      bb.max[k] = std::max(bb.max[k], v[k]);
    }
  }
  return bb;
}
