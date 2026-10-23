/*
 * MeshTransform.h
 *
 * Non-destructive uniform scaling and rotation of a mesh. The original
 * vertices are kept, so the transform can be changed or reset at any time
 * without accumulating rounding errors.
 */

#ifndef TETRAMESHTOOLS_MESHTRANSFORM_H_
#define TETRAMESHTOOLS_MESHTRANSFORM_H_

#include <vector>

#include "TetraToolsExports.h"
#include "Vec3f.h"

namespace TetraTools {

class DLL_EXPORT MeshTransform {
 public:
  MeshTransform();

  /// Stores a copy of the untransformed vertices and resets the transform.
  /// The pivot is the center of their bounding box.
  void SetOriginal(const std::vector<Vec3f> &vertices_);
  /// Drops the original vertices and resets the transform.
  void Clear();
  bool HasOriginal() const { return !original.empty(); }
  const std::vector<Vec3f> &GetOriginal() const { return original; }

  /// Uniform scale factor, must be > 0 (other values are ignored).
  void SetScale(float scale_);
  float GetScale() const { return scale; }
  /// Rotation in degrees about the X, Y and Z axis (applied in this order).
  void SetRotation(float xDeg_, float yDeg_, float zDeg_);
  Vec3f GetRotation() const { return rotation; }
  /// Back to scale 1 and no rotation, keeps the original vertices.
  void Reset();
  bool IsIdentity() const;

  Vec3f Apply(const Vec3f &v_) const;
  /// Writes the transformed original vertices to out_ (resized as needed).
  void Apply(std::vector<Vec3f> &out_) const;

 private:
  void UpdateMatrix();

  std::vector<Vec3f> original;
  Vec3f pivot;
  float scale;
  Vec3f rotation;
  float m[3][3];  // rotation * scale
};

}  // namespace TetraTools

#endif /* TETRAMESHTOOLS_MESHTRANSFORM_H_ */
